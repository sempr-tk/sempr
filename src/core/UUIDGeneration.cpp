#include <sempr/core/UUIDGeneration.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace sempr { namespace core {

boost::uuids::random_generator UUIDGeneration::uuidGenerator_;

std::string UUIDGeneration::generate(const std::string& prefix)
{
    std::string id = boost::uuids::to_string(UUIDGeneration::uuidGenerator_());
    if (usePrefix_) id = prefix + "_" + id;
    return id;
}

}}
