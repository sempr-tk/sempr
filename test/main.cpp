#include <sempr/sempr.hpp>
using namespace sempr::storage;

#include <sempr/entity/CoffeeMug.hpp>
using namespace sempr::entity;

#include <sempr/processing/DebugModule.hpp>
#include <sempr/processing/DBUpdateModule.hpp>
#include <sempr/processing/ActiveObjectStore.hpp>
using namespace sempr::processing;

#include <fstream>
#include <iostream>
#include <boost/uuid/uuid.hpp>

#include <odb/database.hxx>

int main(int argc, char** args)
{
    // test init of discriminator
    DBObject::Ptr p1(new DBObject());
    Entity::Ptr p2(new Entity());
    RDFEntity::Ptr pRDF(new RDFEntity());
    Person::Ptr p3(new Person(pRDF));

    std::cout << p1->discriminator() << '\n';
    std::cout << p2->discriminator() << '\n';
    std::cout << p3->discriminator() << '\n';
    // return 0;
    //-----


    ODBStorage::Ptr storage( new ODBStorage() );
    DebugModule::Ptr debug( new DebugModule() );
    DBUpdateModule::Ptr updater( new DBUpdateModule(storage) );
    ActiveObjectStore::Ptr active( new ActiveObjectStore() );

    sempr::core::Core c(storage);
    c.addModule(active);
    c.addModule(debug);
    c.addModule(updater);

    {
        CoffeeMug::Ptr mug1 = std::make_shared<CoffeeMug>();
        c.addEntity(mug1);

        RDFEntity::Ptr personRDF(new RDFEntity());
        c.addEntity(personRDF);

        Person::Ptr person1 = std::make_shared<Person>(personRDF);
        c.addEntity(person1);
        active->printStats();
        // c.removeEntity(person1);
        // active->printStats();
    }


    {
        std::cout << "LOADING" << '\n';
        // show all entities
        std::vector<DBObject::Ptr> entities;
        storage->loadAll(entities);
        for (DBObject::Ptr e : entities) {
            std::cout << "loaded: " << e->discriminator() << " " << e->uuid() << '\n';
        }
        std::cout << "END LOADING" << '\n';
    }

    //
    // boost::uuids::uuid mug_id;
    // {
    //     CoffeeMug::Ptr mug1 = std::make_shared<CoffeeMug>();
    //     c.addEntity(mug1);
    //
    //     std::cout << "Mug temp: " << mug1->temperature_ << '\n';
    //     mug1->temperature_ = mug1->temperature_ - 1;
    //     std::cout << "Mug temp: " << mug1->temperature_ << '\n';
    //     mug1->temperature_ = mug1->temperature_ - 1;
    //     std::cout << "Mug temp: " << mug1->temperature_ << '\n';
    //     mug1->temperature_ = mug1->temperature_ - 1;
    //     std::cout << "Mug temp: " << mug1->temperature_ << '\n';
    //
    //     for (int i = 0; i < mug1->temperature_.size(); i++) {
    //         std::cout << "temp[" << i << "]: " << mug1->temperature_[i] << '\n';
    //     }
    //
    //     mug_id = mug1->uuid();
    //     mug1->changed();
    // }
    //
    // {
    //     CoffeeMug::Ptr mug = std::dynamic_pointer_cast<CoffeeMug>(storage->load(mug_id));
    //     std::cout << "loaded mug, temperature: " << mug->temperature_ << '\n';
    //
    //     Person::Ptr p = std::make_shared<Person>();
    //     p->first_ = "Max"; p->last_ = "Mustermann";
    //     p->age_ = 45;
    //     p->gender_ = Person::MALE;
    //     c.addEntity(p);
    //
    //     p->age_ = 46;
    //     p->changed();
    //
    //     mug->owner_ = p;
    //     // storage->save(mug);
    //     mug->changed();
    //
    //     Person::Ptr p2 = std::make_shared<Person>();
    //     p2->first_ = "Someone"; p2->last_ = "Else";
    //     p2->age_ = 27;
    //     c.addEntity(p2);
    //     mug->owner_ = p2;
    //     mug->changed();
    //
    // }



    return 0;
}
