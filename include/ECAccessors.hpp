#ifndef SEMPR_ECACCESSORS_HPP_
#define SEMPR_ECACCESSORS_HPP_

#include <rete-core/Accessors.hpp>
#include <rete-rdf/TriplePart.hpp>

#include "ECWME.hpp"
#include "Utility.hpp"

namespace sempr {

/**
    ECWMEs are only a typedef to TupleWME<Entity::Ptr, Component::Ptr>.
*/
//typedef rete::TupleWMEAccessor<0, ECWME> EntityAccessor;

/**
    Extracts the Entity-Part from an ECWME, as an Entity::Ptr, a std::string
    which is just the EntityId, or a TriplePart which is a URI of the form <sempr:EntityId>.
*/
class EntityAccessor : public rete::Accessor<ECWME, Entity::Ptr,
                                             rete::TriplePart, std::string> {
public:
    using Ptr = std::shared_ptr<EntityAccessor>;
    EntityAccessor();

    EntityAccessor* clone() const override;
    std::string toString() const override;
    bool equals(const AccessorBase& other) const override;

    void getValue(ECWME::Ptr, Entity::Ptr& value) const override;
    void getValue(ECWME::Ptr, rete::TriplePart& value) const override;
    void getValue(ECWME::Ptr, std::string& value) const override;

};

/**
    ComponentAccessors are a bit more complicated, as we will want to differ
    between different component types.
*/
template <class C>
class ComponentAccessor :
        public rete::Accessor<ECWME, std::shared_ptr<C>, Component::Ptr> {
public:
    using Ptr = std::shared_ptr<ComponentAccessor>;

    /**
        ComponentAccessors are only equal if they refer to the same component
        class. The class type already specifies exactly what is accessed, so
        nothing else needs to be checked.
    */
    bool equals(const rete::AccessorBase& other) const override
    {
        auto o = dynamic_cast<const ComponentAccessor*>(&other);
        if (o)
            return true;
        else
            return false;
    }


    /**
        Implements getValue of the ValueAccessor. Grants access to the concrete
        Component type, instead of just the sempr::Component.
    */
    void getValue(ECWME::Ptr wme, std::shared_ptr<C>& value) const override
    {
        // get the component
        auto component = std::get<1>(wme->value_);
        // assume its the type we want
        value = std::static_pointer_cast<C>(component);
    }

    void getValue(ECWME::Ptr wme, Component::Ptr& value) const override
    {
        value = std::get<1>(wme->value_);
    }


    // must be cloneable
    ComponentAccessor* clone() const override
    {
        auto a = new ComponentAccessor();
        a->index_ = this->index_;
        return a;
    }


    std::string toString() const override
    {
        return ComponentName<C>::value + (this->index_ < 0 ? std::string("") : "[" + std::to_string(this->index_) + "]");
    }
};

// specialization for ComponentAccessor<Component::Ptr>, else the doubled
// template argument causes error
template <>
class ComponentAccessor<Component> :
        public rete::Accessor<ECWME, Component::Ptr> {
public:
    using Ptr = std::shared_ptr<ComponentAccessor>;

    bool equals(const rete::AccessorBase& other) const override
    {
        auto o = dynamic_cast<const ComponentAccessor*>(&other);
        if (o)
            return true;
        else
            return false;
    }

    void getValue(ECWME::Ptr wme, Component::Ptr& value) const override
    {
        value = std::get<1>(wme->value_);
    }

    ComponentAccessor* clone() const override
    {
        auto a = new ComponentAccessor();
        a->index_ = this->index_;
        return a;
    }

    std::string toString() const override
    {
        return "Component" + (this->index_ < 0 ? std::string("") : "[" + std::to_string(this->index_) + "]");
    }
};


/**
    Accessor to get the tag of a component
*/
class ComponentTagAccessor : public rete::Accessor<ECWME, std::string> {
public:
    void getValue(ECWME::Ptr wme, std::string& value) const override;

    bool equals (const rete::AccessorBase& other) const override;
    ComponentTagAccessor* clone() const override;
    std::string toString() const override;
};

}
#endif /* include guard: SEMPR_ECACCESSORS_HPP_ */
