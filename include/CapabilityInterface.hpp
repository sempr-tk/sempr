#ifndef SEMPR_CAPABILITY_INTERFACE_HPP_
#define SEMPR_CAPABILITY_INTERFACE_HPP_

#include <QtPlugin>


namespace sempr {
    class Core;

/**
    A capability in sempr is a group of functionality provided by one or more
    builtins/effects for the underlying rete reasoner. It is an attempt to
    reduce the amount of manual registrations of node builders in an application
    and instead load capability plugins which handle this task.
*/
class CapabilityInterface {
public:
    virtual ~CapabilityInterface() = default;

    /**
        Implements the capability at the given core, by e.g. registering the
        node builders.
    */
    virtual void setup(Core* core) const = 0;
};


/**
    The CapabilityBuilderInterface is the actual plugin that can be loaded by
    sempr and is only used to construct a new instance of a capability. This is
    done to prevent side-effects between different instances of the sempr core
    using the same plugins. E.g., the RDFPlugin contains a soprano instance. If
    it was to be used by multiple sempr instances, the triples from both would
    be combined in the same soprano module. Hence, this is split, such that
    each sempr core gets its own instances of the capabilities.
*/
class CapabilityBuilderInterface {
public:
    virtual ~CapabilityBuilderInterface() = default;

    /**
        Creates a new capability. Ownership is transferred to the caller.
    */
    virtual CapabilityInterface* create() const = 0;
};

}

Q_DECLARE_INTERFACE(sempr::CapabilityBuilderInterface, "sempr.CapabilityBuilderInterface/1.0")


#endif /* include guard: SEMPR_CAPABILITY_INTERFACE_HPP_ */
