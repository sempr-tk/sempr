#include <sempr/core/IncrementalIDGeneration.hpp>
#include <PrefixAssignedIDs_odb.h>

#include <sstream>

namespace sempr { namespace core {

IncrementalIDGeneration::IncrementalIDGeneration(storage::ODBStorage::Ptr st)
{
    using entity::PrefixAssignedIDs;
    storage_ = st;
    storage = st;
    try {
        prefixInfo_ =
        storage->load<PrefixAssignedIDs>("IncIDGen_config");
    } catch (odb::exception& e) {
        std::cout << "Couldn't load config for IncrementalIDGeneration." << '\n';
        std::cout << "Creating new one." << '\n';
        prefixInfo_ = PrefixAssignedIDs::Ptr(
            new PrefixAssignedIDs(new PredefinedID("IncIDGen_config"))
        );
        storage->save(prefixInfo_);
    }
}

IncrementalIDGeneration::~IncrementalIDGeneration()
{
    storage->save(prefixInfo_);
}


#include <iostream>

std::string IncrementalIDGeneration::generate(const std::string& prefix) {
    size_t id = prefixInfo_->getIDForPrefix(prefix);
    std::string fullID = prefix + "_" + std::to_string(id);

    // storage::ODBStorage::Ptr storage = storage_.lock();
    if (!storage) {
        throw std::runtime_error("IncrementalIDGeneration: ID requested, " \
                                 "but storage-object is already destroyed!");
    }
    // storage->save(prefixInfo_);
    std::cout << "generated id: " << fullID << '\n';
    return fullID;
}

void IncrementalIDGeneration::revoke(const std::string &prefix, const std::string& id)
{
    if (!storage) {
        throw std::runtime_error("IncrementalIDGeneration: ID to revoke, " \
                                 "but storage-object is already destroyed!");
    }
    std::istringstream iss(id.substr(prefix.size()+1));
    size_t i;
    iss >> i;
    prefixInfo_->revokeID(prefix, i);
    std::cout << "revoked id: " << prefix << ", " << id << '\n';
}

}}
