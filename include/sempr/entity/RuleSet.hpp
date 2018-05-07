#ifndef SEMPR_ENTITY_RULESET_HPP_
#define SEMPR_ENTITY_RULESET_HPP_

#include <sempr/entity/Entity.hpp>
#include <sempr/core/RDF.hpp>
#include <map>
#include <vector>
#include <string>
#include <memory>

namespace sempr { namespace entity {

/**
    A RuleSet represents (semantic) rules that have to be interpreted by an RDF-reasoner (in this
    case: the rule engine of soprano). It provides a few default-prefixes (rdf, rdfs, sempr, ...)
    like the SPARQLQuery that are to be resolved by the rule reasoner.
    The rules themselves are stored as strings in the form of:
        [name: (s1, p1, o1), (s2, p2, o2) -> (s3, p3, o3), (s4, p4, o4)]
*/
#pragma db object
class RuleSet : public Entity {
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<RuleSet>;

    std::map<std::string, std::string> prefixes;
    std::vector<std::string> rules;

    RuleSet();
    RuleSet(const core::IDGenBase*);

    void add(const std::string& rule) {
        rules.push_back(rule);
    }
};

}}

#endif /* end of include guard SEMPR_ENTITY_RULESET_HPP_ */
