#ifndef SEMPR_ECACCESSORS_HPP_
#define SEMPR_ECACCESSORS_HPP_

#include <rete-core/Accessors.hpp>
#include "ECWME.hpp"
#include "Utility.hpp"

namespace sempr {

/**
    ECWMEs are only a typedef to TupleWME<Entity::Ptr, Component::Ptr>.
*/
//typedef rete::TupleWMEAccessor<0, ECWME> EntityAccessor;
//class EntityAccessor : public rete::TupleWMEAccessor<0, ECWME> {
class EntityAccessor : public rete::StringAccessor,
                       public rete::ValueAccessor<Entity::Ptr, EntityAccessor> {
public:
    using Ptr = std::shared_ptr<EntityAccessor>;
    EntityAccessor();

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

    using rete::StringAccessor::getValue;
    using rete::ValueAccessor<Entity::Ptr, EntityAccessor>::getValue;
    void getValue(rete::WME::Ptr, Entity::Ptr& value) const override;
    void getValue(rete::WME::Ptr, std::string& value) const override;

    bool equals(const Accessor& other) const override;
};

/**
    ComponentAccessors are a bit more complicated, as we will want to differ
    between different component types.

    NOTE: When writing nodes that expect a Component to work on, consider to
    *not* expect a ComponentAccessor<MyComponent>, but rather a
                   SpecificTypeAccessor<std::shared_ptr<MyComponent>>.
    The latter is more general, and the ComponentAccessor already derives from
    SpecificTypeAccessor. Therefore, if we have a builtin that creates a
    component it can export it in a TupleWME, and the component can then be
    used in your node. The ComponentAccessor would restrict it to components
    that are part of an ECWME.
*/
template <class C>
class ComponentAccessor : public rete::SpecificTypeAccessor<std::shared_ptr<C>>,
                          public rete::ValueAccessor<std::shared_ptr<C>, ComponentAccessor<C>> {
public:
    using Ptr = std::shared_ptr<ComponentAccessor>;
    ComponentAccessor()
    {
        this->template registerType<ComponentAccessor>();
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
        // assume its a ECWME
        auto ecwme = std::static_pointer_cast<ECWME>(wme);
        // get the component
        auto component = std::get<1>(ecwme->value_);
        // assume its the type we want
        value = std::static_pointer_cast<C>(component);
    }

    /**
        Checks if the two accessors can compare their values
    */
    bool canCompareValues(const rete::Accessor& other) const override
    {
        return other.canAs<ComponentAccessor>();
    }

    /**
        Checks if the same component instance is referred to
    */
    bool valuesEqual(rete::Accessor& other,
                     rete::Token::Ptr token,
                     rete::WME::Ptr wme) override
    {
        auto optr = other.as<ComponentAccessor>();
        std::shared_ptr<C> myValue, otherValue;

        if (this->index() == -1) this->getValue(wme, myValue);
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
        return ComponentName<C>::value + (this->index_ < 0 ? std::string("") : "[" + std::to_string(this->index_) + "]");
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
