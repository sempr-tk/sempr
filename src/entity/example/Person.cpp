#include <sempr/entity/example/Person.hpp>
#include <sempr/entity/Triple.hpp>
#include <Person_odb.h>
#include <sempr/core/RDF.hpp>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(Person)

Person::Person(const core::IDGenBase* idgen)
    : Entity(idgen)
{
    setDiscriminator<Person>();
    prop_ = RDFPropertyMap::Ptr(new RDFPropertyMap(*this));
    // std::cout << id() << " created an RDFPropertyMap, namely " << prop_->id() << '\n';

    registerChildEntity(prop_);

    RDFPropertyMap& m = *prop_;
    m["name"] = "Max Mustermann";
    m["age"] = 42;
    m["height"] = 1.85;

    // RDFEntity::Ptr basic(new RDFEntity());
    // basic->addTriple(Triple( "<" + sempr::baseURI() + this->id() + ">", sempr::core::rdf::type(), "<" + sempr::baseURI() + "Person>"));
    // registerChildEntity(basic);
    m("type", core::rdf::baseURI()) = RDFResource("<" + sempr::baseURI() + "Person" + ">");
}

Person::Person() : Person(new core::IDGen<Person>())
{
}


} /* entity */

} /* sempr */
