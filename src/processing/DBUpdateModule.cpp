#include <sempr/processing/DBUpdateModule.hpp>

namespace sempr { namespace processing {

DBUpdateModule::DBUpdateModule(storage::Storage::Ptr storage)
    : storage_(storage)
{
    addOverload<entity::Entity::Event>(
        [this](entity::Entity::Event::Ptr e) {
            process(e);
        }
    );
}

void DBUpdateModule::process(entity::Entity::Event::Ptr e)
{
    if (e->what() == core::EntityEventBase::CHANGED)
    {
        //std::cout << "UPDATER: SAVING " << e->getEntity()->id() << '\n';
        storage_->save(e->getEntity());
    }
}

}}
