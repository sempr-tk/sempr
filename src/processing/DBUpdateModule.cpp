#include <sempr/processing/DBUpdateModule.h>
#include <iostream>

namespace sempr { namespace processing {

DBUpdateModule::DBUpdateModule(storage::Storage::Ptr storage)
    : storage_(storage)
{
    addOverload<core::EntityEvent>(
        [this](core::EntityEvent::Ptr e) {
            process(e);
        }
    );
}

void DBUpdateModule::process(core::EntityEvent::Ptr e)
{
    if (e->what() == core::EntityEvent::CHANGED)
    {
        std::cout << "UPDATER: SAVING " << e->getEntity()->id() << '\n';
        storage_->save(e->getEntity());
    }
}

}}
