#ifndef SEMPR_CORE_SEPARATEFILESTORAGE_HPP_
#define SEMPR_CORE_SEPARATEFILESTORAGE_HPP_

#include <boost/filesystem.hpp>
#include <set>
#include <string>

#include "Storage.hpp"
#include "IDGenerator.hpp"

namespace sempr {
    namespace fs = boost::filesystem;

/**
    The SeparateFileStorage is an implementation of the Storage interface.
    It makes use of the cereal serialization of entities and components to save
    them locally on the disk -- on entity per file, containing all its
    components.

    It also implements the IDGenerator interface, as it makes sense to combine
    these two functionalities: It knows of all the entities that are stored,
    even when they are not loaded, and avoids issuing the same id twice.
*/
class SeparateFileStorage : public Storage, public IDGenerator {
    fs::path dirPath_; // path to the directory where entities shall be saved.

    std::set<std::string> entityIDs_; // all ever given entity ids, to avoid
                                      // collisions

    size_t nextID_; // the next id to try to issue
public:
    using Ptr = std::shared_ptr<SeparateFileStorage>;
    /**
        Creates a new storage -- everything will be saved to / loaded from the
        given directory.
        Throws an exception of the given dir is not a directory.
    */
    SeparateFileStorage(const std::string& dir);

    /**
        Saves the entity in its whole.
    */
    void save(Entity::Ptr) override;

    /**
        Delegates to save(Entity::Ptr), this storage only stores entities as a
        whole.
    */
    void save(Entity::Ptr, Component::Ptr) override;


    /**
        Creates a new ID for the entity
    */
    std::string createIDFor(Entity::Ptr) override;

    /**
        Allows to re-assign the id of the entity
    */
    void releaseIDOf(Entity::Ptr) override;
};



}

#endif /* include guard: SEMPR_CORE_SEPARATEFILESTORAGE_HPP_ */

