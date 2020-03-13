#include "util/FileWatcher.hpp"

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <errno.h>
#include <poll.h>

#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN    (1024*(EVENT_SIZE + 16))

namespace sempr {

FileWatcher::~FileWatcher()
{
    stop();
}

void FileWatcher::stop()
{
    exitSignal_.set_value();
    if (watcher_.joinable())
        watcher_.join();
}

void FileWatcher::start(const std::string& path, FileWatcher::callback_t callback)
{
    stop();
    exitSignal_ = std::promise<void>();
    std::future<void> f = exitSignal_.get_future();
    watcher_ = std::thread(&FileWatcher::run, this, path, f.share(), callback);
}



// helper: discard any remaining events on the inotify descriptor
void discardRemainingEvents(int fd)
{
    char buffer[BUF_LEN];

    // polling setup for non-blocking read
    struct pollfd pfd;
    int num_pfd = 1;
    int timeout_msecs = 10;
    pfd.fd = fd;
    pfd.events = POLLIN;
    int pollRet;
    // only as long as poll gives us data to read
    while ((pollRet = poll(&pfd, num_pfd, timeout_msecs)) > 0)
    {
        read(fd, buffer, BUF_LEN); // just read, do nothing.
    }
}


// run parallel, in a thread.
void FileWatcher::run(const std::string& path, std::shared_future<void> exitSignal, FileWatcher::callback_t callback)
{
    int fd = inotify_init(); // inotify instance

    // try to get the watcher manually, non-blocking first, just to throw
    // the NOT_EXISTS event if it fails.
    int wd = inotify_add_watch(fd, path.c_str(),
                    IN_MODIFY |
                    IN_CREATE | IN_MOVED_TO |
                    IN_IGNORED | IN_MOVE_SELF);
    if (wd < 0) {
        callback(Event::NOT_EXISTS, exitSignal);
    }
    else
    {
        callback(Event::EXISTS, exitSignal);
    }

    // as long as we do not get the exit-signal
    while (exitSignal.wait_for(std::chrono::milliseconds(1)) == std::future_status::timeout)
    {
        // if the initial try succeeded, reuse it, else (wd < 0) try the blocking
        // variant to get it, and then throw the exists.
        if (wd < 0)
        {
            // try to get the watcher on the specified path
            wd = waitForExistAndGetWatch(fd, path, exitSignal);
            if (wd >= 0)
            {
                // success! this is worth an EXISTS event
                callback(Event::EXISTS, exitSignal);
            }
            else
            {
                // well... this should not happen. Either the exit signal was sent
                // during waitForExistAndGetWatch (which we would get in the next
                // pass of the loops condition, no worries), or something internally
                // messed up. In that case, it's better to stop everything here.
                // TODO expose some kind of error value to the outside!
                break;
            }
        }

        while (wd >= 0)
        {
            // wait for more events, check exit signal in the meantime
            struct pollfd pfd;
            int num_pfd = 1;
            int timeout_msecs = 50;
            pfd.fd = fd;
            pfd.events = POLLIN;
            int pollRet;
            while ((pollRet = poll(&pfd, num_pfd, timeout_msecs)) == 0)
            {
                // while in this loop, all is good but there is no new data
                // so check the exit-signal.
                if (exitSignal.wait_for(std::chrono::milliseconds(1)) != std::future_status::timeout)
                {
                    // exit signal given! cleanup and return.
                    inotify_rm_watch(fd, wd);
                    close(fd);
                    return;
                }
            }

            // check pollRet and the returned event for errors
            if (pollRet < 0 || pfd.revents & POLLERR || pfd.revents & POLLNVAL)
            {
                std::cerr << "FileWatcher: Something bad happened during poll." << std::endl;
                // what to do now? cleanup and close. Maybe a custom event
                // for this? Or just NOT_EXISTS assumption? ...
                inotify_rm_watch(fd, wd);
                close(fd);
                return;
            }

            // all is good, and there is data to read
            char buffer[BUF_LEN];
            int length = read(fd, buffer, BUF_LEN);
            if (length < 0)
            {
                std::cerr << "FileWatcher: Something bad happened during read." << std::endl;
                inotify_rm_watch(fd, wd);
                close(fd);
                return;
            }

            // interpret the data. We are interested if the file was
            // modified, moved or removed.
            int i = 0;
            while (i < length)
            {
                struct inotify_event* event = reinterpret_cast<struct inotify_event*>(&buffer[i]);
                if (event->wd == wd) // discards events for other (older) watches.
                {
                    if (event->mask & IN_IGNORED)
                    {
                        // the watch was removed for some reason, e.g. file was
                        // deleted. -> NOT_EXISTS, but stay alert for if it
                        // is created again
                        callback(Event::NOT_EXISTS, exitSignal);
                        // setting wd < 0 breaks the outer loop that works while the file exists
                        wd = -1;
                        // discard the remaining packets on the fd before moving on
                        discardRemainingEvents(fd);
                        break; // skip the rest of the events.
                    }
                    else if (event->mask & IN_MOVE_SELF)
                    {
                        // the file was moved away.
                        callback(Event::NOT_EXISTS, exitSignal);
                        wd = -1;
                        discardRemainingEvents(fd);
                        break;
                    }
                    else if (event->mask & IN_MODIFY)
                    {
                        callback(Event::MODIFIED, exitSignal);
                    }
                }
                else
                {
                    // this is some debug output I added to understand
                    // some weird behaviour when modifying file with vim.
                    // turns out, vim does a lot of stuff when saving a
                    // file: we get an IN_MOVE_SELF, which triggers the
                    // above code to callback(NOT_EXISTS) and get a new wd,
                    // but we also get multiple ignored (probably my own
                    // inotify_rm_watch calls) and a IN_MODIFY on the
                    // originally watched file. What the hell? It was moved
                    // (away?)! What happens there?
                    // At least doing
                    //      echo "foo" >> baz.txt
                    // Only triggers an IN_MODIFY...
                    // NOTE: Just be aware that you will probably get
                    // NOT_EXISTS + EXISTS in most cases when you just
                    // want an MODIFIED.
                    /*
                    std::cout << "event on old wd?!" << std::endl;
                    if (event->mask & IN_IGNORED)
                        std::cout << " ignored" << std::endl;
                    if (event->mask & IN_MOVE_SELF)
                        std::cout << " move_self" << std::endl;
                    if (event->mask & IN_MODIFY)
                        std::cout << " modify" << std::endl;
                    if (event->mask & IN_MOVED_TO)
                        std::cout << " moved_to" << std::endl;
                    if (event->mask & IN_CREATE)
                        std::cout << " create" << std::endl;
                    if (event->len)
                        std::cout << " name: " << event->name << std::endl;
                    */
                }
                i += EVENT_SIZE + event->len;
            } // end while (there are still events to process)
        } // end while (the file still exists / the watch is still good)
    } // end while (no exit signal) -> retry to get the watch

    close(fd);
}

int FileWatcher::waitForExistAndGetWatch(int fd, const std::string& path, std::shared_future<void> exitSignal)
{
    int wd = inotify_add_watch(fd, path.c_str(),
                IN_MODIFY |               // a file/folder is modified, which is what we ultimately want to listen for
                IN_CREATE | IN_MOVED_TO | // a file is created here or moved to to this location.
                                          //Need to listen for this when the file we look for does not exist yet
                IN_IGNORED |              // signals that the watch was removed for some reason,
                                          // e.g. when the watched file is deleted
                IN_MOVE_SELF);            // when the watched folder/file is moved, this does not trigger an
                                          // ignore, but we want to treat it as such.
    if (wd >= 0)
        return wd; // all good.

    // else: failed, so lets watch the parent folder until the file exists
    size_t slash_pos = path.find_last_of("/");
    if (slash_pos == std::string::npos)
    {
        // there is no / in the path... that's bad. An invalid filename, huh?
        std::cerr << "FileWatcher: Invalid path: " << path << std::endl;
        return -1;
    }
    std::string parentFolder = path.substr(0, slash_pos);
    std::string waitingFor = path.substr(parentFolder.length()+1);

    // endless loop: allows us to retry this block if the parentWd that we
    // get hold of at the beginning becomes invalid (if the parent folder we
    // waited for, and existed at some point, get removed before we even get
    // the file we were waiting for).
    // Loop is broken as soon as "path" exists and we get a watch for it.
    while (true)
    {
        bool parentGone = false;
        // recursive -- make sure that parent folder exists.
        int parentWd = waitForExistAndGetWatch(fd, parentFolder, exitSignal);
        if (parentWd < 0)
        {
            // waitForExistAndGetWatch only returns if a wd was successfully
            // created (>= 0), or when the exit signal was triggered (-1).
            return parentWd;
        }

        // try once again to create the watch directly. This might work if
        // a nested path was created at once, e.g. with mkdir -p foo/bar/baz.
        // In that case, we get an IN_CREATE event for foo, but by the time
        // we register a watch for foo the get the creation of bar, bar
        // already exists!
        wd = inotify_add_watch(fd, path.c_str(),
                IN_MODIFY |
                IN_CREATE | IN_MOVED_TO |
                IN_IGNORED |
                IN_MOVE_SELF);
        if (wd > 0)
        {
            inotify_rm_watch(fd, parentWd);
            return wd;
        }


        // since it exists, we now wait until the file/folder we searched for
        // exists and we can get a watch.
        while (!parentGone)
        {
            // use poll to wait for events on the inotify descriptor,
            // and check for the exit signal in between.
            struct pollfd pollDescriptor;
            int numPollDescriptors = 1;
            int timeout_msecs = 50; // check for exitsignal every 50 msecs.
            pollDescriptor.fd = fd;
            pollDescriptor.events = POLLIN;

            int pollRet;
            while ((pollRet = poll(&pollDescriptor, numPollDescriptors, timeout_msecs)) == 0 )
            {
                // no new events, so check if the exit signal was given yet
                if (exitSignal.wait_for(std::chrono::milliseconds(1)) != std::future_status::timeout)
                {
                    // did not time-out, so we need to abort!
                    // -> close the parentWd, and return -1 to signal an invalid return.
                    inotify_rm_watch(fd, parentWd);
                    return -1;
                }
            }

            if (pollRet < 0)
            {
                std::cerr << "FileWatcher: pollRet < 0" << std::endl;
                // some error happened, e.g. a signal was catched.
                // just return -1 to signal that something happened.
                inotify_rm_watch(fd, parentWd);
                return -1;
            }

            if (pollDescriptor.revents & POLLERR || pollDescriptor.revents & POLLNVAL)
            {
                std::cerr << "FileWatcher: POLLERR | POLLNVAL" << std::endl;
                // again, some error condition
                inotify_rm_watch(fd, parentWd);
                return -1;
            }


            char buffer[BUF_LEN];
            int length = read(fd, buffer, BUF_LEN);
            if (length < 0)
            {
                std::cerr << "FileWatcher: Read failed?!" << std::endl;
                inotify_rm_watch(fd, parentWd);
                return -1;
            }

            // something happend
            int i = 0;
            while (i < length)
            {
                struct inotify_event* event = reinterpret_cast<struct inotify_event*>(&buffer[i]);

                // we are listening to a directory, waiting for a file/subdir
                // to be created.
                // what can happen here?
                // 0. it can be an old event (e.g. IN_IGNORED) for another watch descriptor. In that case: Ignore it.
                // 1. the watch is removed for some reason (file/folder deleted, moved?) -> try again...
                // 2. the searched file/folder is created or moved here, nice!
                // 3. something unrelated, don't care.

                // 1.
                if (event->mask & IN_IGNORED && event->wd == parentWd)
                {
                    // ... damn. clean up, and retry...
                    // dont inotify_rm_watch, as that would trigger more IGNORED events!
                    parentGone = true;
                    break;
                }
                else if (event->mask & IN_MOVE_SELF)
                {
                    inotify_rm_watch(fd, parentWd); // triggers a IN_IGNORED event :(
                    parentGone = true;
                    break;
                }
                // 2.
                else if (event->mask & IN_CREATE || event->mask & IN_MOVED_TO)
                {
                    // check if the file created matches the one we are looking for
                    std::string fname(event->name);
                    if (fname == waitingFor)
                    {
                        inotify_rm_watch(fd, parentWd);
                        // just call this function again, with same parameters.
                        // gets us the watch, and handles problems if that fails for some reason.
                        return waitForExistAndGetWatch(fd, path, exitSignal);
                    }
                }
                // 3.
                else
                {
                    /*
                    std::cout << "something else, unrelated, happened";
                    if (event->len) std::cout << " with " << std::string(event->name);
                    std::cout << std::endl;
                    */
                }
                i += EVENT_SIZE + event->len;
            } // end while (iterating events) of a single read
        } // end while (read after read, as long as the parent exists)
    } // end while (tries to get the parent dir)
}

}
