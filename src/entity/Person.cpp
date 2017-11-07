#include <sempr/entity/Person.hpp>
#include <sempr/entity/Triple.hpp>
#include <Person_odb.h>
#include <sempr/core/RDF.hpp>

namespace sempr { namespace entity {

Person::Person(RDFEntity::Ptr rdf)
    : rdfEntity_(rdf)
{
    setDiscriminator<Person>();
    // clear the rdf and set some basic values.
    rdfEntity_->clear();
    rdfEntity_->addTriple(Triple(id(), core::rdf::type(), "http://example.com/Person"));
    rdfEntity_->addTriple(Triple(id(), "http://example.com/age", std::to_string(age_)));
    rdfEntity_->addTriple(Triple(id(), "http://example.com/gender",
        (gender_ == Person::MALE ? "m" : (gender_ == Person::FEMALE ? "f" : "unknown"))
    ));
}

// void Person::getTriples(std::vector<core::Triple> &triples) const {
//     using namespace core;
//     Triple t(id(), rdf::type(), "http://example.com/#Person");
//     triples.push_back(t);
//
//     t.predicate = "http://example.com/#age";
//     t.object = std::to_string(age_);
//     triples.push_back(t);
//
//     t.predicate = "http://example.com/#gender";
//     t.object =  (gender_ == Person::MALE ? "m"
//                     : (gender_ == Person::FEMALE ? "f" : "unknown"));
//     triples.push_back(t);
// }

} /* entity */

} /* sempr */
