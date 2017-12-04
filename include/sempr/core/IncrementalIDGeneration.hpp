#ifndef SEMPR_CORE_INCREMENTALIDGENERATION_HPP_
#define SEMPR_CORE_INCREMENTALIDGENERATION_HPP_

#include <memory>
#include <string>
#include <map>
#include <vector>

#include <sempr/storage/ODBStorage.hpp>
#include <sempr/storage/DBObject.hpp>
#include <sempr/entity/PrefixAssignedIDs.hpp>
#include <sempr/core/IDGenUtil.hpp>

namespace sempr { namespace core {

/**
    A IDGenerationStrategy that assigns incremental ID, e.g.
    "Chair_1", "Chair_2", ...
    It uses a custom entity type to store information about already assigned
    IDs in the database.
*/
class IncrementalIDGeneration : public IDGenerationStrategy {
public:
    /**
        Takes a pointer to the storage module to load/save its own
        configuration-entity.
    */
    IncrementalIDGeneration(storage::ODBStorage::Ptr storage);
    ~IncrementalIDGeneration();

    std::string generate(const std::string& prefix) override;
    void revoke(const std::string& prefix, const std::string& id) override;

private:
    std::weak_ptr<storage::ODBStorage> storage_;
    storage::ODBStorage::Ptr storage;

    // std::map<std::string, entity::PrefixAssignedIDs::Ptr> prefixInfo_;
    entity::PrefixAssignedIDs::Ptr prefixInfo_;
};

}}


#endif /* end of include guard SEMPR_CORE_INCREMENTALIDGENERATION_HPP_ */
