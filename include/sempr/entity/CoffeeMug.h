#ifndef SEMPR_ENTITY_COFFEEMUG_H_
#define SEMPR_ENTITY_COFFEEMUG_H_

#include <sempr/entity/Entity.h>
#include <sempr/entity/Person.h>
#include <sempr/storage/History.h>
#include <odb/core.hxx>

namespace sempr { namespace entity {


#pragma db object
class CoffeeMug : public Entity {
public:
    using Ptr = std::shared_ptr<CoffeeMug>;
    
    CoffeeMug() {
        temperature_ = 100;
    }
    
    storage::History<int> temperature_;
    storage::History<Person::Ptr> owner_;
    
protected:
    friend class odb::access;
};

}}


#endif /* end of include guard: SEMPR_ENTITY_COFFEEMUG_H_ */
