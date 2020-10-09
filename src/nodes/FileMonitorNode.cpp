#include "nodes/FileMonitorNode.hpp"
#include <mutex>
#include <iostream>

namespace sempr {

FileMonitorNode::FileMonitorNode(
        std::recursive_mutex& mutex,
        rete::PersistentInterpretation<std::string> acc)
    :
        rete::Builtin("file:exists"),
        reteMutex_(mutex), fileName_(std::move(acc))
{
}



rete::WME::Ptr FileMonitorNode::process(rete::Token::Ptr)
{
    // nothing to do, will never be called. See leftActivate instead.
    return nullptr;
}



// helper: start a watcher
void FileMonitorNode::startWatcher(rete::Token::Ptr token, const std::string& file)
{
    std::recursive_mutex& mutex = reteMutex_;
    auto mem = bmem_;
    watchers_[token].start(file,
        [this, &mutex, mem, token](FileWatcher::Event event, std::shared_future<void> exitSignal) -> void
        {
            // Try to get exclusive access to the rete network in order to
            // propagate the change, but abort if the exitSignal is set
            while (!mutex.try_lock())
            {
                if (exitSignal.wait_for(std::chrono::milliseconds(1)) != std::future_status::timeout)
                {
                    // exit signal was given, so abort this.
                    return;
                }
            }

            // make sure to release the lock at the end in any case!
            std::lock_guard<std::recursive_mutex> lock(mutex, std::adopt_lock);

            auto bmem = mem.lock();
            if (!bmem) throw std::exception(); // should never happen.

            if (event == FileWatcher::EXISTS)
            {
                // file exists now, so propagate an ASSERT
                auto empty = std::make_shared<rete::EmptyWME>();
                this->setComputed(empty, true);
                bmem->leftActivate(token, empty, rete::PropagationFlag::ASSERT);
            }
            else if (event == FileWatcher::MODIFIED)
            {
                // it was modified -> UPDATE
                auto empty = std::make_shared<rete::EmptyWME>();
                this->setComputed(empty, true);
                bmem->leftActivate(token, empty, rete::PropagationFlag::UPDATE);
            }
            else if (event == FileWatcher::NOT_EXISTS)
            {
                // file no longer exists -> RETRACT
                bmem->leftActivate(token, nullptr, rete::PropagationFlag::RETRACT);
            }
        }
    );
}


void FileMonitorNode::leftActivate(rete::Token::Ptr token, rete::PropagationFlag flag)
{
    auto bmem = bmem_.lock();
    if (!bmem) throw std::exception(); // no memory to forward anything to, should not be possible.

    if (flag == rete::PropagationFlag::RETRACT)
    {
        std::cout << "FileMonitorNode: RETRACT" << std::endl;
        // first stop the watcher, then emit retract.
        watchers_[token].stop();
        watchers_.erase(token);

        bmem->leftActivate(token, nullptr, rete::PropagationFlag::RETRACT);
    }
    else if (flag == rete::PropagationFlag::UPDATE)
    {
        std::string file;
        fileName_.interpretation->getValue(token, file);
        if (file == watchers_[token].getCurrentPath())
        {
            // fileName did not change, so lets check what the current state of
            // the file is and decide if we drop the update or propagate it.
            FileWatcher::Event lastEvent = watchers_[token].getLastEvent();
            if (lastEvent == FileWatcher::EXISTS || lastEvent == FileWatcher::MODIFIED)
            {
                // file exists, propagate update
                auto empty = std::make_shared<rete::EmptyWME>();
                setComputed(empty, true);
                bmem->leftActivate(token, empty, rete::PropagationFlag::UPDATE);
            }
            else if (lastEvent == FileWatcher::NOT_EXISTS || lastEvent == FileWatcher::NONE)
            {
                // file does not exist *OR* the thread has not yet announced its
                // existance. For the rete it's the same, we just drop the
                // update.
            }
        }
        else
        {
            // so,... the filename changed.
            // That means we have to retract the old information and setup a new
            // watcher.
            watchers_[token].stop();
            bmem->leftActivate(token, nullptr, rete::PropagationFlag::RETRACT);
            startWatcher(token, file);
        }
    }
    else if (flag == rete::PropagationFlag::ASSERT)
    {
        std::cout << "FileMonitorNode: ASSERT" << std::endl;
        // Add a new watcher and do nothing else. The watcher will at some point
        // trigger an assert if the file exists, but will do so asynchronously.
        std::string file;
        fileName_.interpretation->getValue(token, file);
        startWatcher(token, file);
    }
}




bool FileMonitorNode::operator == (const rete::BetaNode& other) const
{
    auto o = dynamic_cast<const FileMonitorNode*>(&other);
    if (!o) return false;

    return *(o->fileName_.accessor) == *(this->fileName_.accessor);
}


}
