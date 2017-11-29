#ifndef SEMPR_CORE_IDGENERATOR_HPP_
#define SEMPR_CORE_IDGENERATOR_HPP_

#include <memory>
#include <sempr/core/IDGenerationStrategy.hpp>
#include <sempr/core/UUIDGeneration.hpp>

namespace sempr { namespace core {
/**
    The IDGenerator is a (singleton) class that is used to create unique ids
    for different types of entities. It uses the type of entity to create a
    prefix-string and forwards it to a concrete IDGenerationStrategy.
*/
class IDGenerator {
private:
    /// pointer to the current strategy.
    std::unique_ptr<IDGenerationStrategy> strategy_;

    /// private ctor (singleton!) with a default strategy.
    IDGenerator() : strategy_(new UUIDGeneration()) {}

public:
    static IDGenerator& getInstance() {
        static IDGenerator gen;
        return gen;
    }

    /**
        Set a new strategy and transfer ownership to the IDGenerator
    */
    void setStrategy(std::unique_ptr<IDGenerationStrategy> strategy) {
        strategy_ = std::move(strategy);
    }

    /**
        Create a new ID for a given type of entity.
    */
    template <class Entity>
    std::string generate() {
        // TODO get a better prefix
        return strategy_->generate(typeid(Entity).name());
    }
};

/**
    Base class for the ID-Generation-Wrapper. The generate-method can actually
    be const since it won't modify itself, but only the ID-generation-instance.
*/
struct IDGenBase {
    virtual std::string generate() const = 0;
};

/**
    A convenience wrapper for use in entity classes. For a correct IDGeneration,
    every Entity-Class must accept a IDGenBase-object that wraps the entity-type
    and the IDGenerator-instance. Use it like this:
        SomeEntity(IDGenBase& gen = IDGen<SomeEntity>())
            : Entity(gen)
        {
            setDiscriminator<SomeEntity>();
            // ctor of SomeEntity
        }
    Far down the hierarchy, DBObject's ctor will use the IDGenerator to set its
    id. Therefore, it will be already set inside the ctors of the derived
    classes and can be used when creating sub-entities, creating relations, etc.
*/
template <class Entity>
struct IDGen : public IDGenBase {
    std::string generate() const override {
        return IDGenerator::getInstance().generate<Entity>();
    }
};

} /* core */
} /* sempr */


#endif /* end of include guard: SEMPR_CORE_IDGENERATOR_HPP_ */
