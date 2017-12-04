#include <sempr/entity/PrefixAssignedIDs.hpp>
#include <PrefixAssignedIDs_odb.h>

namespace sempr { namespace entity {

PrefixAssignedIDs::PrefixAssignedIDs(const core::IDGenBase* idgen)
    : Entity(idgen)
{
    setDiscriminator<PrefixAssignedIDs>();
}

size_t PrefixAssignedIDs::getIDForPrefix(const std::string &prefix)
{
    PrefixIDInfo& info = prefixInfos_[prefix];

    // return a revoked id if possible
    if (!info.revoked.empty()) {
        size_t id = *info.revoked.begin();
        info.revoked.erase(info.revoked.begin());
        return id;
    }

    // else, increment the highest
    return ++info.highest;
}

void PrefixAssignedIDs::revokeID(const std::string &prefix, size_t id)
{
    PrefixIDInfo& info = prefixInfos_[prefix];
    if (info.revoked.find(id) == info.revoked.end()) {
        info.revoked.insert(id);
    }
}


}}
