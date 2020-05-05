#ifndef SEMPR_FILEMONITORNODE_HPP_
#define SEMPR_FILEMONITORNODE_HPP_

#include <rete-core/Builtin.hpp>
#include <rete-core/Accessors.hpp>
#include <rete-core/TupleWME.hpp>
#include <rete-core/Token.hpp>

#include <mutex>
#include <map>

#include "../util/FileWatcher.hpp"

namespace sempr {

/**
    The FileMonitorNode expects a filename and only passes the token if the file
    exists. It furthermore installs a monitor on the file in order to ASSERT
    tuples for which the file is created later on, RETRACT those for which the
    file is deleted, and issue a CHANGED event when a file is modified. These
    monitors run in separate threads. To make this process thread-safe, they
    need to aquire a lock on the rete network.
*/
class FileMonitorNode : public rete::Builtin {
    // a mutex to lock before triggering stuff in the rete network from the
    // monitoring threads
    std::recursive_mutex& reteMutex_;

    // mapping of tokens to watchers
    std::map<rete::Token::Ptr, FileWatcher> watchers_;

    // accessor for the filename
    std::unique_ptr<rete::StringAccessor> fileName_;

    // starts a watcher for the given token.
    void startWatcher(rete::Token::Ptr, const std::string& file);

public:
    /**
        Creates a new FileMonitorNode. This will spawn a monitor for every
        token that arrives at this node, which will watch the file
        referenced by the given StringAccessor. The token will be forwarded
        (ASSERT) if and when the file exists, held back if the file does not
        exist, RETRACTed if the file stops existing, and UPDATEd when the file
        is modified.

        Please note: All these activations in the rete network will happen
        asynchronously, by the threads that monitor the files. Correct behaviour
        is enforced by serializing these accesses through a given mutex. Please
        make sure to lock this mutex if you make external changes (adding data
        etc), too!

        Also, be aware: This means that the effects of rules that include this
        node are not immediately available after adding the required
        preconditions! First, the threads monitoring the files have to react.
        For that, they need to be able to aquire the lock, which at the point of
        adding the data, you are holding yourself. This is especially important
        to know for performInference(): You must aquire the lock before calling
        performInference(), but that means that the threads will obviously not
        be able to propagate their results (even if they could, there would be
        race conditions). So, "performInference" usually iterates until
        everything that can be inferred is inferred -- but it will fail to do
        so here.

        TODO: There might be a solution: If we check for the existence of the
        file right away without waiting for the thread we should be good for the
        immediate inference, and the monitors could still update the knowledge
        base later on.
    */
    FileMonitorNode(std::recursive_mutex&, std::unique_ptr<rete::StringAccessor>);


    /**
        ASSERT:  Register a new watcher for the token.
        RETRACT: Remove the watcher for the token, propagate RETRACT
        UPDATE:  If the filename did not change: Propagate UPDATE
                 If the filename changed: Stop the watcher, propagate RETRACT,
                                          start the watcher (triggers ASSERT if
                                          file exists).

        One note on the implementation detail:
        The body of this method is actually empty. The reason is that the
        rete::Builtin class is designed to make the common case easy -- the
        common case being simple calculations. That means that the management
        of when to call process and whether to propagate RETRACT, ASSERT or
        UPDATE is done by the rete::Builtin. For this node we need this to be
        a little different, as e.g. it explicitely needs to know of retracted
        things in order to remove the file watchers. To do this, we can still
        override the leftActivate-method and bypass the rete::Builtins default
        behaviour --> see below.
    */
    rete::WME::Ptr process(rete::Token::Ptr) override;

    /**
        See doc for process(rete::Token::Ptr) for a detailed description.
    */
    void leftActivate(rete::Token::Ptr, rete::PropagationFlag) override;


    /**
        Equality-check for possible reusability of nodes
    */
    bool operator == (const rete::BetaNode& other) const override;
};


}

#endif /* include guard: SEMPR_FILEMONITORNODE_HPP_ */
