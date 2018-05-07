#ifndef SEMPR_PROCESSING_DBUPDATEMODULE_H_
#define SEMPR_PROCESSING_DBUPDATEMODULE_H_

#include <sempr/core/EntityEvent.hpp>
#include <sempr/processing/Module.hpp>
#include <sempr/storage/Storage.hpp>
#include <sempr/entity/Entity.hpp>

namespace sempr { namespace processing {

class DBUpdateModule : public Module {
public:
    using Ptr = std::shared_ptr<DBUpdateModule>;
    DBUpdateModule(storage::Storage::Ptr storage);
    
    /** upon receiving a changed-event, update the database */
    void process(core::EntityEventBase::Ptr e);
    
private:
    storage::Storage::Ptr storage_;
};

}}

#endif /* end of include guard: SEMPR_PROCESSING_DBUPDATEMODULE_H_ */
