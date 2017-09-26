#include <sempr/entity/Person.h>

namespace sempr { namespace entity {

void Person::getTriples(std::vector<core::Triple> &triples) const {
    using namespace core;
    Triple t(id(), rdf::type(), "http://example.com/#Person");
    triples.push_back(t);
    
    t.predicate = "http://example.com/#age";
    t.object = std::to_string(age_);
    triples.push_back(t);
    
    t.predicate = "http://example.com/#gender";
    t.object =  (gender_ == Person::MALE ? "m" 
                    : (gender_ == Person::FEMALE ? "f" : "unknown"));
    triples.push_back(t);
}

} /* entity */
    
} /* sempr */
