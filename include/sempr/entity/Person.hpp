#ifndef SEMPR_ENTITY_PERSON_H_
#define SEMPR_ENTITY_PERSON_H_

#include <sempr/entity/Entity.hpp>
#include <sempr/storage/History.hpp>
// #include <sempr/core/RDF.hpp>
#include <sempr/entity/RDFEntity.hpp>

namespace sempr { namespace entity {

#pragma db object
class Person : public RDFEntity {
public:
    using Ptr = std::shared_ptr<Person>;
    enum Gender { MALE, FEMALE, UNKNOWN };

    Person();
    virtual ~Person(){}

    unsigned int age_; // TODO replace with a timestamp "born"
    std::string first_, last_; /// name
    Gender gender_;

    /** return symbolic knowledge about this person **/
    void getTriples(std::vector<core::Triple>& triples) const override;

protected:
    friend class odb::access;
};

    // enable history:
    typedef storage::History<Person::Ptr> PersonHistory;
    #pragma db value(PersonHistory)

}}

#endif /* end of include guard: SEMPR_ENTITY_PERSON_H_ */
