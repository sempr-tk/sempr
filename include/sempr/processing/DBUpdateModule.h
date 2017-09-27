#ifndef SEMPR_PROCESSING_DBUPDATEMODULE_H_
#define SEMPR_PROCESSING_DBUPDATEMODULE_H_

#include <sempr/core/EntityEvent.h>
#include <sempr/processing/Module.h>
#include <sempr/storage/Storage.h>

namespace sempr { namespace processing {

class DBUpdateModule : public Module {
public:
    using Ptr = std::shared_ptr<DBUpdateModule>;
    DBUpdateModule(storage::Storage::Ptr storage);
    
    /** upon receiving a changed-event, update the database */
    void process(core::EntityEvent::Ptr e);
    
private:
    storage::Storage::Ptr storage_;
};

}}

#endif /* end of include guard: SEMPR_PROCESSING_DBUPDATEMODULE_H_ */
