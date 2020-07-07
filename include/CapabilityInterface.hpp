#ifndef SEMPR_CAPABILITY_INTERFACE_HPP_
#define SEMPR_CAPABILITY_INTERFACE_HPP_

#include <QtPlugin>

#include "Core.hpp"

namespace sempr {

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
    virtual void setup(Core* core) const;
};

}

Q_DECLARE_INTERFACE(sempr::CapabilityInterface, "sempr.CapabilityInterface/1.0")


#endif /* include guard: SEMPR_CAPABILITY_INTERFACE_HPP_ */
