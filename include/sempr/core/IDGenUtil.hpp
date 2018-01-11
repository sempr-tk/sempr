#ifndef SEMPR_CORE_IDGENUTIL_HPP_
#define SEMPR_CORE_IDGENUTIL_HPP_

#include <sempr/core/IDGenerator.hpp>

namespace sempr { namespace core {

/**
    Base class for the ID-Generation-Wrapper. The generate-method can actually
    be const since it won't modify itself, but only the ID-generation-instance.
*/
struct IDGenBase {
    virtual std::string generate() const = 0;
    virtual void revoke(const std::string& id) const = 0;
};

/**
    A convenience wrapper for use in entity classes. For a correct IDGeneration,
    every Entity-Class must accept a IDGenBase-object that wraps the entity-type
    and the IDGenerator-instance. Use it like this:
        SomeEntity(core::IDGenBase& gen)
            : Entity(gen)
        {
            setDiscriminator<SomeEntity>();
            // ctor of SomeEntity
        }
        SomeEntity() : SomeEntity(core::IDGen<SomeEntity>()) {}
    Far down the hierarchy, DBObject's ctor will use the IDGenerator to set its
    id. Therefore, it will be already set inside the ctors of the derived
    classes and can be used when creating sub-entities, creating relations, etc.
*/
template <class Entity>
struct IDGen : public IDGenBase {
    std::string generate() const override {
        return IDGenerator::getInstance().generate<Entity>();
    }

    void revoke(const std::string& id) const override {
        IDGenerator::getInstance().revoke<Entity>(id);
    }
};

/**
    A naive way of generating IDs: Just create them manually. This is used
    e.g. by IDGeneration-strategies to load/store specific information at a
    predefined place. Else we'd get some problems when we want to create a
    configuration-object, but need to create an ID to do so, but need a
    configuration-object to create the ID, ...
    Hence, predefined IDs.
*/
struct PredefinedID : public IDGenBase {
    std::string id_;
    PredefinedID(const std::string& id) : id_(id) {}

    std::string generate() const override {
        return id_;
    }

    void revoke(const std::string& id) const override {
        // nothing to do.
    }
};

}}

#endif /* end of include guard SEMPR_CORE_IDGENUTIL_HPP_ */
