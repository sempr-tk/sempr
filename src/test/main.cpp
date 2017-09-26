#include <sempr/sempr.h>
using namespace sempr::storage;

#include <sempr/entity/CoffeeMug.h>
using namespace sempr::entity;

#include <sempr/processing/DebugModule.h>
using namespace sempr::processing;

#include <fstream>
#include <iostream>
#include <boost/uuid/uuid.hpp>


#include <unordered_map>
#include <typeinfo>
#include <typeindex>
#include <functional>
#include <typeinfo>

class Base {public: virtual ~Base(){} };
class A : public Base {};
class B : public Base {};

class BModule {
public:
    template<typename D, typename F> void AddOverload(F f) {
        // insert pair (type --> lambda calling f plus a static_cast) into map
        types_[typeid(D)] = [f](std::shared_ptr<Base> event) {
            f(std::static_pointer_cast<D>(event));
        };
        std::cout << "added " << typeid(D).name()  << '\n';
    }
    
    virtual void process(std::shared_ptr<Base> event) {
        if (types_.find(typeid(*event)) != types_.end()) {
            types_[typeid(*event)](event);
        } else {
            std::cout << "couldnt find " << typeid(*event).name() << '\n';
        }
    }
    
private:
    std::unordered_map<
        std::type_index, 
        std::function<void(std::shared_ptr<Base>)> > types_;
};

class PrintModule : public BModule {
public:
    PrintModule() {
        // AddOverload<std::shared_ptr<A> >( &PrintModule::printA );
        AddOverload<A>([this](std::shared_ptr<A> blub) {
            printA(blub);
            std::cout << "GOT AN A, YAY" << '\n';
        });
    }
    
    void printA(std::shared_ptr<A> a) {
        std::cout << "GOT AN A" << '\n';
    }
};



int main(int argc, char** args)
{
    auto base = std::make_shared<Base>();
    auto a = std::make_shared<A>();
    auto b = std::make_shared<B>();

    PrintModule pr;
    pr.process(base);
    pr.process(a);

    // return 0;
    ODBStorage::Ptr storage( new ODBStorage() );
    DebugModule::Ptr debug( new DebugModule() );
    
    sempr::core::Core c(storage);
    c.addModule(debug);
    
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
        
        storage->save(p);
        mug->owner_ = p;
        storage->save(mug);
    }
    
    
    
    return 0;
}
