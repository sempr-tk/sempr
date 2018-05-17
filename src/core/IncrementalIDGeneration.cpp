#include <PrefixAssignedIDs_odb.h>
#include <sempr/core/IncrementalIDGeneration.hpp>

#include <iostream>
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
        std::cerr << "Couldn't load config for IncrementalIDGeneration. ";
        std::cerr << "Creating new one." << '\n';
        prefixInfo_ = PrefixAssignedIDs::Ptr(
            new PrefixAssignedIDs(new PredefinedID("IncIDGen_config"))
        );
        storage->save(prefixInfo_);
    }
}

IncrementalIDGeneration::~IncrementalIDGeneration()
{
    // explicitly save every entry, as we don't rely on any event-mechanism here.
    // the id-generation is singleton and thus the last light to go out...
    for (auto e : *prefixInfo_)
    {
        storage->save(e.second);
    }

    storage->save(prefixInfo_);
}


std::string IncrementalIDGeneration::generate(const std::string& prefix) {
    size_t id = prefixInfo_->getIDForPrefix(prefix);
    std::string fullID = prefix + "_" + std::to_string(id);

    if (!storage) {
        throw std::runtime_error("IncrementalIDGeneration: ID requested, " \
                                 "but storage-object is already destroyed!");
    }
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
}

}}
