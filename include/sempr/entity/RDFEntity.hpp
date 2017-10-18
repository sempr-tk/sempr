#ifndef SEMPR_ENTITY_RDFENTITY_H_
#define SEMPR_ENTITY_RDFENTITY_H_

#include <sempr/core/RDF.hpp>
#include <sempr/entity/Entity.hpp>
#include <sempr/core/EventBroker.hpp>
#include <sempr/core/EntityEvent.hpp>

namespace sempr { namespace entity {

#pragma db object
class RDFEntity : public Entity {
public:
    using Event = core::EntityEvent<RDFEntity>;
    virtual void getTriples(std::vector<core::Triple>& triples) const = 0;
    
    ENTITY_DEFAULT_EVENT_METHODS(RDFEntity, Entity);
};

}}

#endif /* end of include guard: SEMPR_ENTITY_RDFENTITY_H_ */
