#ifndef SEMPR_ENTITY_PREFIXASSIGNEDIDS_HPP_
#define SEMPR_ENTITY_PREFIXASSIGNEDIDS_HPP_

#include <sempr/entity/Entity.hpp>
#include <odb/core.hxx>
// #include <queue>
#include <set>

namespace sempr { namespace entity {

    /**
        The PrefixIDInfo is a simple helper-class that holds information about
        the IDs that have been created for a specific (implicit) prefix:
        - highest: The highest ever assigned ID
        - revoked: A list of IDs that are free to be reused
        Since this is needed for the ID-generation it must not use the
        ID-generation itself. The PrefixAssignedIDs-class is responsible to
        manage these objects and assign them fixed IDs.
    */
    #pragma db object // value not possible, container inside container not allowed!
    class PrefixIDInfo : public Entity {
    public:
        using Ptr = std::shared_ptr<PrefixIDInfo>;
        PrefixIDInfo(const core::IDGenBase*);

        // std::string prefix;
        std::size_t highest;
        std::set<size_t> revoked;

    private:
        friend class odb::access;
        PrefixIDInfo();
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


        /**
            make the PrefixAssignedID iterable to allow the IncrementalIDGeneration to save every
            entry manually without fireing events and hoping for the best, but using the storage-
            module itself for the task.
        */
        std::map<std::string, PrefixIDInfo::Ptr>::iterator begin() { return prefixInfos_.begin(); }
        std::map<std::string, PrefixIDInfo::Ptr>::iterator end() { return prefixInfos_.end(); }

    protected:
        // update the map entries, too!
        // TODO: Discuss: Should this be a feature of the
        // child-handling-capability of Entity? Update all children on update
        // of the parent? <--> changed()-events lead to updates through the
        // DBUpdateModule. Do we really need that module?
        void preUpdate(odb::database& db) const override;

    private:
        PrefixAssignedIDs() {}

        friend class odb::access;
        std::map<std::string, PrefixIDInfo::Ptr> prefixInfos_;
    };

}}

#endif /* end of include guard SEMPR_ENTITY_PREFIXASSIGNEDIDS_HPP_ */
