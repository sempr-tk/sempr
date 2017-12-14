#ifndef SEMPR_ENTITY_RDFENTITY_H_
#define SEMPR_ENTITY_RDFENTITY_H_

#include <odb/core.hxx>
#include <sempr/core/RDF.hpp>
#include <sempr/entity/Entity.hpp>
#include <sempr/entity/Triple.hpp>

// #include <sempr/core/EventBroker.hpp>
#include <sempr/core/EntityEvent.hpp>

#include <vector>

namespace sempr { namespace entity {

#pragma db object
class RDFEntity : public Entity {
public:
    using Ptr = std::shared_ptr<RDFEntity>;
    using Event = core::EntityEvent<RDFEntity>;
    RDFEntity();
    RDFEntity(const core::IDGenBase*);
    virtual ~RDFEntity(){}

    void getTriples(std::vector<Triple>& triples) const;
    Triple& getTripleAt(const size_t& index);
    void addTriple(const Triple& triple);
    void removeTriple(const Triple& triple);
    void removeTripleAt(const size_t& index);
    void clear();
    size_t size() const;
    std::vector<Triple>::const_iterator begin() const;
    std::vector<Triple>::const_iterator end() const;


    ENTITY_DEFAULT_EVENT_METHODS(RDFEntity, Entity);

protected:
    friend class odb::access;
    std::vector<Triple> triples_;
};

}}

#endif /* end of include guard: SEMPR_ENTITY_RDFENTITY_H_ */
