#include <sempr/entity/Person.hpp>
#include <sempr/entity/Triple.hpp>
#include <Person_odb.h>
#include <sempr/core/RDF.hpp>

namespace sempr { namespace entity {

Person::Person() : rdfEntity_(new RDFEntity())
{
    setDiscriminator<Person>();
    registerChildEntity(rdfEntity_);
    init();
}


void Person::init() {
    // clear the rdf and set some basic values.
    rdfEntity_->clear();
    rdfEntity_->addTriple(Triple(id(), core::rdf::type(), "http://example.com/Person"));
    rdfEntity_->addTriple(Triple(id(), "http://example.com/age", std::to_string(age_)));
    rdfEntity_->addTriple(Triple(id(), "http://example.com/gender",
        (gender_ == Person::MALE ? "m" : (gender_ == Person::FEMALE ? "f" : "unknown"))
    ));
}


} /* entity */

} /* sempr */
