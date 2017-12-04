#ifndef SEMPR_CORE_IDGENERATOR_HPP_
#define SEMPR_CORE_IDGENERATOR_HPP_

#include <memory>
#include <sempr/core/IDGenerationStrategy.hpp>
#include <sempr/core/UUIDGeneration.hpp>

#include <odb/core.hxx>

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


    template <class Entity>
    std::string getPrefix()
    {
        // for a prefix, use the odb-generated discriminator
        std::string prefix =
            odb::object_traits_impl<Entity, odb::id_common>::info.discriminator;

        // for readability remove the namespaces
        auto pos = prefix.rfind("::");
        if (pos != prefix.npos) {
            prefix = prefix.substr(pos+2);
        }

        return prefix;
    }

    /**
        Create a new ID for a given type of entity.
    */
    template <class Entity>
    std::string generate() {
        std::string prefix = getPrefix<Entity>();

        return strategy_->generate(prefix);
    }

    /**
        Revoke an existing ID
    */
    template <class Entity>
    void revoke(const std::string& id) {
        std::string prefix = getPrefix<Entity>();
        strategy_->revoke(prefix, id);
    }
};


} /* core */
} /* sempr */


#endif /* end of include guard: SEMPR_CORE_IDGENERATOR_HPP_ */
