#include <sempr/sempr.hpp>
using namespace sempr::storage;

#include <sempr/entity/CoffeeMug.hpp>
#include <sempr/entity/RDFPropertyMap.hpp>
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
    ODBStorage::Ptr storage( new ODBStorage() );
    DebugModule::Ptr debug( new DebugModule() );
    DBUpdateModule::Ptr updater( new DBUpdateModule(storage) );
    ActiveObjectStore::Ptr active( new ActiveObjectStore() );

    sempr::core::Core c(storage);
    c.addModule(active);
    c.addModule(debug);
    c.addModule(updater);


    {
        // Person::Ptr p(new Person());
        // c.addEntity(p);
        //
        // RDFPropertyMap::Ptr map(new RDFPropertyMap(*p));
        // c.addEntity(map);
        // (*map)["age"] = 5.; // double value
        // (*map)["name"] = std::string("Max");
        // (*map)["weight"] = 20.3;
        //
        // RDFPropertyMap& m = *map;
        // int age = m["age"];
        // m["age"] = age; // overwrite original double with int --> change of type!
        //
        // std::string ages = m["age"];
        // std::cout << "string-age: " << ages << '\n';
        //
        // std::string ages2 = m["age"];
        // std::cout << "string-age: " << ages2 << '\n';
        //
        // // try a pointer!
        // m["person-obj"] = p;

        std::vector<DBObject::Ptr> objects;
        storage->loadAll(objects);
        for (auto o : objects) {
            RDFPropertyMap::Ptr m = std::dynamic_pointer_cast<RDFPropertyMap>(o);
            if (m.get()) {
                std::cout << "got RDFPropertyMap:" << '\n';
                // std::cout << (*m)["age"] << '\n'; // error.
                // std::cout << (*m)["name"] << '\n';
                // std::cout << (*m)["weight"] << '\n';
                int age = (*m)["age"];
                std::string name = (*m)["name"];
                double weight = (*m)["weight"];
                Person::Ptr person = (*m)["person-obj"];
                std::cout << age << '\n';
                std::cout << name << '\n';
                std::cout << weight << '\n';
                std::cout << "person-obj: " << person->id() << '\n';

                // get invalid
                RDFPropertyMap::Ptr invalid = (*m)["person-obj"];
                std::cout << "propertymap: " << invalid.get()  << '\n';
                invalid = (*m)["foobarbaz"];
                std::cout << "propertymap: " << invalid.get()  << '\n';
            }
        }
    }


    return 0;
}
