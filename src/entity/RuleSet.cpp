#include <sempr/entity/RuleSet.hpp>
#include <RuleSet_odb.h>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(RuleSet)

RuleSet::RuleSet(const core::IDGenBase* idgen)
    : Entity(idgen)
{
    setDiscriminator<RuleSet>();

    using namespace sempr::core;
    prefixes["rdf"] = core::rdf::baseURI();
    prefixes["rdfs"] = rdfs::baseURI();
    prefixes["owl"] = owl::baseURI();
    prefixes["sempr"] = sempr::baseURI();
    prefixes["xsd"] = xsd::baseURI();
}

RuleSet::RuleSet() : RuleSet(new core::IDGen<RuleSet>()){
}

}}
