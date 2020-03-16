#ifndef SEMPR_UTIL_FILEWATCHER_HPP_
#define SEMPR_UTIL_FILEWATCHER_HPP_

#include <functional>
#include <thread>
#include <future>
#include <mutex>

namespace sempr {

/**
    The FileWatcher uses inotify (linux only!) to monitor a file specified by
    its full path. As soon as the file is seen to exist, it calls the given
    callback with an EXISTS flag. When the file is renamed, moved or deleted,
    or the inotify-watcher becomes invalid for some other reason, NOT_EXISTS
    is triggered. If the file is modified, you get the MODIFIED flag.

    Please note: "MODIFIED" is not what you intuitively think it is.
    E.g., when you edit a file in vim and save it with ":w", files get moved
    around. What you get is NOT_EXISTS + EXISTS shortly after one another.
    Though, doing an
        echo "hello, world!" >> foo.txt
    Correctly triggers MODIFIED (given foo.txt existed already).
*/
class FileWatcher {
public:
    enum Event { EXISTS, MODIFIED, NOT_EXISTS, NONE };
    typedef std::function<void(FileWatcher::Event, std::shared_future<void>)> callback_t;
private:
    /// The currently running thread.
    std::thread watcher_;

    /// The currently watched path
    std::string path_;

    /// A promise for an exit signal. The running thread checks this at some
    /// points in its execution through a std::future object. It is used to
    /// gracefully stop the thread.
    std::promise<void> exitSignal_;

    /// A mutex for the lastEvent variable
    mutable std::mutex lastEventMutex_;
    /// A buffer for the last event. Can be retrieved to actively query for the
    /// state of the file.
    Event lastEvent_;


    void setLastEvent(Event);

    /**
        The run method makes use of waitForExistAndGetWatch to aquire a watch
        on the specified file, and calls the callback when the file is
        (re-)moved or modified. An EXISTS event is called as soon as the watch
        was aquired.
    */
    void run(const std::string& path, std::shared_future<void> exitSignal, callback_t callback);

    /**
        The name speaks for itself: This method tries to get a watch on the file
        or folder given as 'path'. If it fails, it strips the last component of
        'path' (/foo/bar/baz -> /foo/bar), recursively tries to get a watch on
        that and monitors the activity in the folder. If the originally
        requested path comes to existence, it creates and returns the
        inotify-watcher.
    */
    int waitForExistAndGetWatch(int fd, const std::string& path, std::shared_future<void> exitSignal);
public:
    FileWatcher();

    /**
        Stops the monitoring thread (if running)
    */
    ~FileWatcher();

    /**
        Returns the last event issued by the watching thread. May be NONE if
        called directly after starting the thread.
    */
    Event getLastEvent() const;

    /**
        Starts a thread that watches for the given file and calls the callback
        with the EXISTS/MODIFIED/NOT_EXISTS flags when said changes happen.
        The callback also gets access to the exitSignal object to check for
        itself if execution should be aborted. This can be useful if the watcher
        thread is waiting in the callback for the main thread to release a
        resource, but the main thread wants to close the watcher and thus waits
        for the thread to finish (-> deadlock). If that might happen in your
        case, just check for the exit signal in your callback.

        Stops any previously monitoring activity before starting the thread!
    */
    void start(const std::string& path, callback_t callback);

    /**
        Returns the currently watched path, or an empty string
    */
    std::string getCurrentPath() const;

    /**
        Stops the currently running thread.
    */
    void stop();
};

}

#endif /* include guard: SEMPR_UTIL_FILEWATCHER_HPP_ */
