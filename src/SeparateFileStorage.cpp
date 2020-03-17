#include "SeparateFileStorage.hpp"
#include "Exception.hpp"

#include <cereal/archives/json.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>

#include <fstream>
#include <sstream>

namespace sempr {

SeparateFileStorage::SeparateFileStorage(const std::string& dir)
    : dirPath_(dir), nextID_(0)
{
    if (!fs::is_directory(dirPath_))
        throw Exception(dirPath_.string() + " is not a directory.");

    // list all json files, those are our entities
    for (auto& p : fs::directory_iterator(dirPath_))
    {
        if (p.path().extension() == ".json")
        {
            // and the id is just the stem.
            // /foo/bar/Entity_7.json -> Entity_7
            entityIDs_.insert(p.path().stem());
        }
    }
}


void SeparateFileStorage::save(Entity::Ptr entity)
{
    // open a json file in the specified folder...
    fs::path p = dirPath_;
    p /= entity->id() + ".json";
    std::ofstream ofs(p);

    // ...and serialize the entity into it.
    cereal::JSONOutputArchive archive(ofs);
    archive(*entity);
}


void SeparateFileStorage::save(Entity::Ptr entity, Component::Ptr)
{
    save(entity);
}


std::string SeparateFileStorage::createIDFor(Entity::Ptr entity)
{
    // if the entity already has an id, skip this.
    if (!entity->id().empty()) return entity->id();

    // increment nextID_ until an id is found that was not issued yet.
    std::string id;
    do {
        id = "Entity_" + std::to_string(nextID_++);
    } while (entityIDs_.find(id) != entityIDs_.end());

    // remember the id
    entityIDs_.insert(id);
    return id;
}


void SeparateFileStorage::releaseIDOf(Entity::Ptr entity)
{
    entityIDs_.erase(entity->id());

    if (entity->id().length() > 7)
    {
        // extract the numerical part
        std::stringstream ss(entity->id().substr(7)); // strips "Entity_"
        size_t val;
        ss >> val; // parses the rest as a size_t

        // always set nextID_ to the lowest free id, to always reuse the
        // small ones. Leads to a lot of iterating over already assigned ids
        // after a small one was reused, but who cares?
        if (val < nextID_) nextID_ = val;
    }
}

}
