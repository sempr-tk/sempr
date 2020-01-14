#ifndef SEMPR_ECNODE_HPP
#define SEMPR_ECNODE_HPP

#include <utility>

#include <rete-core/AlphaNode.hpp>
#include <rete-core/Util.hpp>
#include <rete-core/Accessors.hpp>

#include "Component.hpp"
#include "Utility.hpp"
#include "ECWME.hpp"

namespace sempr {

/**
    AlphaNodes for EC<Component>(?entity ?component [?tag]) conditions.
    (Actually only for the Entity-Component part. The tag is checked in an
    extra node).
*/
template <class C>
class ECNode : public rete::AlphaNode {
    std::string name_;

    std::string getDOTAttr() const override
    {
        return "[label=\"" + rete::util::dotEscape(name_) + "\"]";
    }

public:
    using Ptr = std::shared_ptr<ECNode>;

    ECNode(const std::string& name)
        : name_(name)
    {
        static_assert(std::is_base_of<Component, C>::value,
                "Component-Part of the desired ECNode is not derived from"
                " sempr::Component");
    }

    ECNode()
        : ECNode(ComponentName<C>::value)
    {
    }


    /**
        Checks if a given WME is of type ECWME and if the component in the
        ECWME is of type C.
    */
    void activate(rete::WME::Ptr wme, rete::PropagationFlag flag) override
    {
        if (flag == rete::PropagationFlag::RETRACT)
        {
            // no need to check on RETRACT. Let the AlphaMemory handle it.
            propagate(wme, rete::PropagationFlag::RETRACT);
            return;
        }

        // check one: is it an ECWME?
        if (ECWME::Ptr ecwme = std::dynamic_pointer_cast<ECWME>(wme))
        {
            // check two: is the contained component of the correct type?
            auto component = std::dynamic_pointer_cast<C>(std::get<1>(ecwme->value_));
            if (component)
            {
                propagate(wme, flag);
            }
        }
    }

    /**
        Equality with another AlphaNode is given if the other has the same type.
        There are no other configs to check that could lead to different
        behaviour.
    */
    bool operator == (const AlphaNode& other) const override
    {
        if (dynamic_cast<const ECNode*>(&other))
        {
            return true;
        }
        return false;
    }
};



/**
    A node just to check for the tag of a component.
    Configure with a string to check for;
    Input a ECWME;
    Output: Passes the wme if the tag of the component in the ECWME equals the
    configured string to check for.
*/
class ComponentTagNode : public rete::AlphaNode {
    std::string getDOTAttr() const override;

    std::string tag_;
public:
    using Ptr = std::shared_ptr<ComponentTagNode>;
    ComponentTagNode(const std::string& tag);
    void activate(rete::WME::Ptr wme, rete::PropagationFlag flag) override;
    bool operator == (const AlphaNode& other) const override;
};

}

#endif /* include guard : SEMPR_ECNODE_HPP */
