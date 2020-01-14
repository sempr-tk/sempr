#ifndef SEMPR_ECACCESSORS_HPP_
#define SEMPR_ECACCESSORS_HPP_

#include <rete-core/TupleWMEAccessor.hpp>
#include "ECWME.hpp"
#include "Utility.hpp"

namespace sempr {

/**
    ECWMEs are only a typedef to TupleWME<Entity::Ptr, Component::Ptr>.
*/
//typedef rete::TupleWMEAccessor<0, ECWME> EntityAccessor;
class EntityAccessor : public rete::TupleWMEAccessor<0, ECWME> {
public:
    using Ptr = std::shared_ptr<EntityAccessor>;

    /*
        Most methods do not need to be overridden, as the EntityAccessor
        only needs to access a specific value in a specific tuple type (ECWME).
        Only the visualization is changed.

        One side effect is that, if at some point some nodebuilder returns a
        Tuple<Entity::Ptr, Component::Ptr>-Accessor (not an EntityAccessor),
        it is considered equal to an EntityAccessor and the latter might be
        overridden in the variable bindings...
        -> The visual representation will change from "EntityAccessor" to
        "TupleAccessor". No big deal.
    */

    /// must be cloneable
    EntityAccessor* clone() const override;

    /// for visualization
    std::string toString() const override;
};

/**
    ComponentAccessors are a bit more complicated, as we will want to differ
    between different component types.
*/
template <class C>
class ComponentAccessor : public rete::TupleWMEAccessor<1, ECWME>,
                          public rete::ValueAccessor<std::shared_ptr<C>, ComponentAccessor<C>> {
public:
    using Ptr = std::shared_ptr<ComponentAccessor>;
    ComponentAccessor()
    {
        registerType<ComponentAccessor>();
    }

    /**
        ComponentAccessors are only equal if they refer to the same component
        class. The class type already specifies exactly what is accessed, so
        nothing else needs to be checked.
    */
    bool equals(const rete::Accessor& other) const override
    {
        return other.canAs<ComponentAccessor>();
    }

    // pull getValue from ValueAccessor into this namespace
    using rete::ValueAccessor<std::shared_ptr<C>, ComponentAccessor>::getValue;


    /**
        Implements getValue of the ValueAccessor. Grants access to the concrete
        Component type, instead of just the sempr::Component.
    */
    void getValue(rete::WME::Ptr wme, std::shared_ptr<C>& value) const override
    {
        Component::Ptr component;
        this->rete::TupleWMEAccessor<1, ECWME>::getValue(wme, component); // use the TupleWMEAccessors method to get it
        // and just cast it to the desired type
        // (static cast, because the node builder that created this accessor
        // knows exactly what the node it inserted in the rete network returns)
        value = std::static_pointer_cast<C>(component);
    }

    /**
        Checks if the two accessors can compare their values
    */
    bool canCompareValues(const Accessor& other) const override
    {
        return other.canAs<ComponentAccessor>();
    }

    /**
        Checks if the same component instance is referred to
    */
    bool valuesEqual(Accessor& other,
                     rete::Token::Ptr token,
                     rete::WME::Ptr wme) override
    {
        auto optr = other.as<ComponentAccessor>();
        std::shared_ptr<C> myValue, otherValue;

        if (index() == -1) this->getValue(wme, myValue);
        else               this->getValue(token, myValue);

        if (optr->index() == -1) optr->getValue(wme, otherValue);
        else                     optr->getValue(token, otherValue);

        return myValue == otherValue;
    }


    // must be cloneable
    ComponentAccessor* clone() const override
    {
        return new ComponentAccessor(*this);
    }


    std::string toString() const override
    {
        return ComponentName<C>::value + (index_ < 0 ? std::string("") : "[" + std::to_string(index_) + "]");
    }
};

/**
    Accessor to get the tag of a component
*/
class ComponentTagAccessor : public rete::StringAccessor {
    bool equals (const rete::Accessor& other) const override;
public:
    void getValue(rete::WME::Ptr wme, std::string& value) const override;
    ComponentTagAccessor* clone() const override;
};

}
#endif /* include guard: SEMPR_ECACCESSORS_HPP_ */
