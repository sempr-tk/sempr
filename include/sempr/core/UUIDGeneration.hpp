#ifndef SEMPR_CORE_UUIDGENERATION_HPP_
#define SEMPR_CORE_UUIDGENERATION_HPP_

#include <sempr/core/IDGenerationStrategy.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace sempr { namespace core {

/**
    A stateless, simple IDGenerationStrategy that uses boosts uuids.
*/
class UUIDGeneration : public IDGenerationStrategy {
public:
    UUIDGeneration(bool usePrefix = true) : usePrefix_(usePrefix) {}
    std::string generate(const std::string& prefix) override;
    void revoke(const std::string& prefix, const std::string& id) override
    {
        // nothing to do.
    }

private:
    bool usePrefix_;
    static boost::uuids::random_generator uuidGenerator_;
};



}}


#endif /* end of include guard SEMPR_CORE_UUIDGENERATION_HPP_ */
