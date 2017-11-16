#ifndef SEMPR_ENTITY_PERSON_H_
#define SEMPR_ENTITY_PERSON_H_

#include <sempr/entity/Entity.hpp>
#include <sempr/storage/History.hpp>
#include <sempr/entity/RDFPropertyMap.hpp>

namespace sempr { namespace entity {

#pragma db object
class Person : public Entity {
public:
    using Ptr = std::shared_ptr<Person>;
    enum Gender { MALE, FEMALE, UNKNOWN };

    Person();
    virtual ~Person(){}

    // getter/setter for name, age and height.
    int age() { return (*prop_)["age"]; }
    void age(int i) { (*prop_)["age"] = i; changed(); prop_->changed(); }

    std::string name() { return (*prop_)["name"]; }
    void name(std::string i) { (*prop_)["name"] = i; changed(); prop_->changed(); }

    float height() { return (*prop_)["height"]; }
    void height(float i) { (*prop_)["height"] = i; changed(); prop_->changed(); }

    ENTITY_DEFAULT_EVENT_METHODS(Person, Entity);

protected:
    friend class odb::access;

    RDFPropertyMap::Ptr prop_;
};

// enable history:
typedef storage::History<Person::Ptr> PersonHistory;
#pragma db value(PersonHistory)

}}

#endif /* end of include guard: SEMPR_ENTITY_PERSON_H_ */
