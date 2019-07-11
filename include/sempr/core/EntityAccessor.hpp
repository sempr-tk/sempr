#ifndef SEMPR_CORE_ENTITYACCESSOR_HPP_
#define SEMPR_CORE_ENTITYACCESSOR_HPP_

#include "EntityWME.hpp"
#include "Utility.hpp"
#include "sempr/entity/Entity.hpp"

#include <rete-core/Accessors.hpp>

namespace sempr {
namespace core {

/**
    An accessor specifically for entity-ids. It does not need to implement the whole interface
    itself, as it really is just another StringAccessor. There is no need for nodes to identify
    it as an EntityIDAccessor, since it really just provides strings.
*/
class EntityIDAccessor : public rete::StringAccessor {
public:
    using Ptr = std::shared_ptr<EntityIDAccessor>;
    /**
        Accesses the ID of the entity. Note that it will **not** return the ID as it is used
        internally in sempr, but in a URI compatible format. So for an entity with the internal id
        "Entity_7" the EntityIDAccessor returns "<sempr:Entity_7>".
    */
    void getValue(rete::WME::Ptr, std::string&) const override;

    // for visualization of the EntityIDAccessor
    std::string toString() const override;

    EntityIDAccessor* clone() const override;

    bool equals(const Accessor& other) const override;
};

/**
    Specialization of the rete::Accessor for entities.
    One can argue that the EntityAccessors should implement StringAccessor to access the entities
    ID, but this
*/
class EntityAccessorBase : public rete::Accessor,
                           public rete::ValueAccessor<entity::Entity::Ptr, EntityAccessorBase> {
public:
    using Ptr = std::shared_ptr<EntityAccessorBase>;
    EntityAccessorBase();

    using rete::ValueAccessor<entity::Entity::Ptr, EntityAccessorBase>::getValue;

    /**
        Access the entity
    */
    void getValue(rete::WME::Ptr, entity::Entity::Ptr&) const override;

    /**
        Equality check on the accessor (not the pointed at data!)
    */
    bool equals(const rete::Accessor& other) const override;

    /**
        returns always false, as the EntityAccessors must never be compared. That would only lead
        to problems like the invalid Accessor-overriding
            (https://git.hb.dfki.de/nniemann/rete/issues/6)
        etc.

        The EntityAccessor is a bit strange, because it does not actually access any data of the
        entities, but only point at the entity itself. The only sensible comparison would be of
        pointer values, which is of no real use, and still exposes the accessor overriding problem.
        So just don't compare EntityAccessors at all, but use EntityIDAccessors instead!
    */
    bool canCompareValues(const Accessor& other) const override;

    /**
        throws an std::exception.
        See "canCompareValues" for an explanation.
    */
    bool valuesEqual(Accessor& other, rete::Token::Ptr token, rete::WME::Ptr wme) override;

    /**
        Accessors must be clonable.
    */
    EntityAccessorBase* clone() const override;

    /**
        A string representation of the accessor for visualization purposes.
    */
    std::string toString() const override;
};


/**
    A templated EntityAccessor which supports better type-checking on the actual entities type.
    On instantiation it uses the type traits provided by odb to mirror the Entity-hierarchy in the
    EntityAccessor-hierarchy. This way the nodes (and node builders) in the rete network can check
    on construction if the entity types match up.
*/
template <class EntityType, class ParentType = typename base_of<EntityType>::type>
class EntityAccessor : public EntityAccessor<ParentType>,
                       public rete::ValueAccessor<EntityType, EntityAccessor<EntityType>> {
public:
    using Ptr = std::shared_ptr<EntityAccessor>;
    EntityAccessor()
    {
        this->template registerType<EntityAccessor>();
    }

    /**
        Access the entity, concrete type.
    */
    void getValue(rete::WME::Ptr wme, std::shared_ptr<EntityType>& entity) const override
    {
        auto ewme = std::static_pointer_cast<EntityWME>(wme);
        entity = std::static_pointer_cast<EntityType>(ewme->entity());
    }
};


/**
    Recursion anchor for the EntityAccessor-template: When "EntityType" is "DBObject", just inherit
    the EntityAccessorBase.
*/
template <>
class EntityAccessor<storage::DBObject, void> : public EntityAccessorBase {
private:
    EntityAccessor() {}
};

} /* core */
} /* sempr */

#endif /* end of include guard: SEMPR_CORE_ENTITYACCESSOR_HPP_ */
