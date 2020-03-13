#include "nodes/FileMonitorNode.hpp"
#include <mutex>

namespace sempr {

FileMonitorNode::FileMonitorNode(std::mutex& mutex, std::unique_ptr<rete::StringAccessor> acc)
    : rete::Builtin("file:exists"),
      reteMutex_(mutex), fileName_(std::move(acc))
{
}



rete::WME::Ptr FileMonitorNode::process(rete::Token::Ptr)
{
    // nothing to do, will never be called. See leftActivate instead.
    return nullptr;
}


void FileMonitorNode::leftActivate(rete::Token::Ptr token, rete::PropagationFlag flag)
{
    auto bmem = bmem_.lock();
    if (!bmem) throw std::exception(); // no memory to forward anything to, should not be possible.

    if (flag == rete::PropagationFlag::RETRACT)
    {
        // first stop the watcher, then emit retract.
        watchers_[token].stop();
        watchers_.erase(token);

        bmem->leftActivate(token, nullptr, rete::PropagationFlag::RETRACT);
    }
    else if (flag == rete::PropagationFlag::ASSERT)
    {
        // Add a new watcher and do nothing else. The watcher will at some point
        // trigger an assert if the file exists, but will do so asynchronously.
        std::mutex& mutex = reteMutex_;
        auto mem = bmem_;
        watchers_[token].start(fileName_->getString(token),
            [&mutex, mem, token](FileWatcher::Event event, std::shared_future<void> exitSignal) -> void
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
                std::lock_guard<std::mutex> lock(mutex, std::adopt_lock);

                auto bmem = mem.lock();
                if (!bmem) throw std::exception(); // should never happen.

                if (event == FileWatcher::EXISTS)
                {
                    // file exists now, so propagate an ASSERT
                    auto empty = std::make_shared<rete::EmptyWME>();
                    bmem->leftActivate(token, empty, rete::PropagationFlag::ASSERT);
                }
                else if (event == FileWatcher::MODIFIED)
                {
                    // it was modified -> UPDATE
                    auto empty = std::make_shared<rete::EmptyWME>();
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
}




bool FileMonitorNode::operator == (const rete::BetaNode& other) const
{
    auto o = dynamic_cast<const FileMonitorNode*>(&other);
    if (!o) return false;

    return *(o->fileName_) == *(this->fileName_);
}


}
