#include <sempr/entity/PrefixAssignedIDs.hpp>
#include <PrefixAssignedIDs_odb.h>

namespace sempr { namespace entity {

PrefixIDInfo::PrefixIDInfo(const core::IDGenBase* idgen)
    : Entity(idgen), highest(0)
{
}

PrefixIDInfo::PrefixIDInfo()
{
}


PrefixAssignedIDs::PrefixAssignedIDs(const core::IDGenBase* idgen)
    : Entity(idgen)
{
    setDiscriminator<PrefixAssignedIDs>();
}

size_t PrefixAssignedIDs::getIDForPrefix(const std::string &prefix)
{
    PrefixIDInfo::Ptr info = prefixInfos_[prefix];
    if (!info) {
        // std::cout << "Create new PrefixIDInfo for " << prefix << '\n';
        info = PrefixIDInfo::Ptr(
            new PrefixIDInfo(new core::PredefinedID("PrefixIDInfo_" + prefix))
        );
        prefixInfos_[prefix] = info;
        registerChildEntity(info);
    }

    // return a revoked id if possible
    if (!info->revoked.empty()) {
        size_t id = *info->revoked.begin();
        info->revoked.erase(info->revoked.begin());
        return id;
    }

    // else, increment the highest
    info->highest++;
    return info->highest;
}

void PrefixAssignedIDs::revokeID(const std::string &prefix, size_t id)
{
    PrefixIDInfo::Ptr info = prefixInfos_[prefix];
    if (!info) {
        throw std::runtime_error("Tried to revoke id of yet unknown prefix.");
    }

    info->revoked.insert(id); // no need to check for duplicates, its a set.
}


void PrefixAssignedIDs::preUpdate(odb::database& db) const
{
    Entity::preUpdate(db);

    for (auto entry : prefixInfos_) {
        db.update(entry.second);
    }
}

}}
