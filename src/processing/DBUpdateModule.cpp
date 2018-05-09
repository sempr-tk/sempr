#include <sempr/processing/DBUpdateModule.hpp>

namespace sempr { namespace processing {

DBUpdateModule::DBUpdateModule(storage::Storage::Ptr storage)
    : storage_(storage)
{
}

void DBUpdateModule::process(core::EntityEvent<entity::Entity>::Ptr e)
{
    if (e->what() == core::EntityEventBase::CHANGED)
    {
        // std::cout << "UPDATER: SAVING " << e->getEntity()->id() << '\n';
        storage_->save(e->getEntity());
    }
}

}}
