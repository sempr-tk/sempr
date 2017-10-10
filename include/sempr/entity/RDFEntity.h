#ifndef SEMPR_ENTITY_RDFENTITY_H_
#define SEMPR_ENTITY_RDFENTITY_H_

#include <sempr/core/RDF.h>
#include <sempr/entity/Entity.h>
#include <sempr/core/EventBroker.h>
#include <sempr/core/EntityEvent.h>

namespace sempr { namespace entity {

#pragma db object
class RDFEntity : public Entity {
public:
    using Event = core::EntityEvent<RDFEntity>;
    virtual void getTriples(std::vector<core::Triple>& triples) const = 0;
    
    ENTITY_DEFAULT_EVENT_METHODS(RDFEntity);
};

}}

#endif /* end of include guard: SEMPR_ENTITY_RDFENTITY_H_ */
