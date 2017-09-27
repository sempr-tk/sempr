#include <sempr/sempr.h>
using namespace sempr::storage;

#include <sempr/entity/CoffeeMug.h>
using namespace sempr::entity;

#include <sempr/processing/DebugModule.h>
#include <sempr/processing/DBUpdateModule.h>
using namespace sempr::processing;

#include <fstream>
#include <iostream>
#include <boost/uuid/uuid.hpp>

int main(int argc, char** args)
{
    ODBStorage::Ptr storage( new ODBStorage() );
    DebugModule::Ptr debug( new DebugModule() );
    DBUpdateModule::Ptr updater( new DBUpdateModule(storage) );
    
    sempr::core::Core c(storage);
    c.addModule(debug);
    c.addModule(updater);
    
    {
        std::cout << "LOADING" << '\n';
        // show all entities
        std::vector<DBObject::Ptr> entities;
        storage->loadAll(entities);
        for (DBObject::Ptr e : entities) {
            std::cout << "loaded: " << e->uuid() << '\n';
        }
        std::cout << "END LOADING" << '\n';
    }
    
    
    boost::uuids::uuid mug_id;
    {
        CoffeeMug::Ptr mug1 = std::make_shared<CoffeeMug>();
        c.addEntity(mug1);
        
        std::cout << "Mug temp: " << mug1->temperature_ << '\n';
        mug1->temperature_ = mug1->temperature_ - 1;
        std::cout << "Mug temp: " << mug1->temperature_ << '\n';
        mug1->temperature_ = mug1->temperature_ - 1;
        std::cout << "Mug temp: " << mug1->temperature_ << '\n';
        mug1->temperature_ = mug1->temperature_ - 1;
        std::cout << "Mug temp: " << mug1->temperature_ << '\n';
        
        for (int i = 0; i < mug1->temperature_.size(); i++) {
            std::cout << "temp[" << i << "]: " << mug1->temperature_[i] << '\n';
        }    
        storage->save(mug1);
        mug_id = mug1->uuid();
        mug1->changed();
    }
    
    {
        CoffeeMug::Ptr mug = std::dynamic_pointer_cast<CoffeeMug>(storage->load(mug_id));
        std::cout << "loaded mug, temperature: " << mug->temperature_ << '\n';
        
        Person::Ptr p = std::make_shared<Person>();
        p->first_ = "Max"; p->last_ = "Mustermann";
        p->age_ = 45;
        p->gender_ = Person::MALE;
        c.addEntity(p);
        
        p->age_ = 46;
        p->changed();
        
        mug->owner_ = p;
        // storage->save(mug);
        mug->changed();
    }
    
    
    
    return 0;
}
