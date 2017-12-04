#ifndef SEMPR_ENTITY_PREFIXASSIGNEDIDS_HPP_
#define SEMPR_ENTITY_PREFIXASSIGNEDIDS_HPP_

#include <sempr/entity/Entity.hpp>
#include <odb/core.hxx>
// #include <queue>
#include <set>

namespace sempr { namespace entity {

    #pragma db value
    struct PrefixIDInfo {
        // std::string prefix;
        std::size_t highest;
        std::set<size_t> revoked;
    };

    /**
        A custom entity for the incremental id generation strategy.
        For every prefix used the highest assigned id and a list of revoked ids
        (which can be re-assigned before incrementing the highest) is stored.
    */
    #pragma db object
    class PrefixAssignedIDs : public Entity {
    public:
        using Ptr = std::shared_ptr<PrefixAssignedIDs>;

        PrefixAssignedIDs(const core::IDGenBase*);

        /**
            Get a free id for a given prefix.
        */
        size_t getIDForPrefix(const std::string& prefix);

        /**
            Revoke an ID in order to reuse it later on in getIDForPrefix.
        */
        void revokeID(const std::string& prefix, size_t id);

    private:
        PrefixAssignedIDs() {}

        friend class odb::access;
        std::map<std::string, PrefixIDInfo> prefixInfos_;
    };

}}

#endif /* end of include guard SEMPR_ENTITY_PREFIXASSIGNEDIDS_HPP_ */
