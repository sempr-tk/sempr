#ifndef SEMPR_ENTITY_RDFVECTOR_H_
#define SEMPR_ENTITY_RDFVECTOR_H_

#include <odb/core.hxx>
#include <sempr/core/RDF.hpp>
#include <sempr/entity/Entity.hpp>
#include <sempr/entity/Triple.hpp>
#include <sempr/entity/RDFEntity.hpp>

// #include <sempr/core/EventBroker.hpp>
#include <sempr/core/EntityEvent.hpp>

#include <vector>

namespace sempr { namespace entity {

// custom triple-iterator for the vector class
class RDFVectorIterator_impl : public TripleIterator_impl {
    friend class RDFVector;
    std::vector<Triple>::const_iterator vit_;

    RDFVectorIterator_impl(std::vector<Triple>::const_iterator it);
    ~RDFVectorIterator_impl();

    const Triple operator * () const override;
    void operator ++ () override;
    bool operator == (const TripleIterator_impl& other) const override;
};



#pragma db object
class RDFVector : public RDFEntity {
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<RDFVector>;
    RDFVector();
    RDFVector(const core::IDGenBase*);
    virtual ~RDFVector(){}

    void getTriples(std::vector<Triple>& triples) const;
    Triple& getTripleAt(const size_t& index);
    bool addTriple(const Triple& triple);
    bool removeTriple(const Triple& triple);
    void removeTripleAt(const size_t& index);
    void clear();
    size_t size() const;

    // std::vector<Triple>::const_iterator begin() const;
    // std::vector<Triple>::const_iterator end() const;

    TripleIterator begin() const override;
    TripleIterator end() const override;

protected:
    friend class odb::access;
    std::vector<Triple> triples_;
};

}}

#endif /* end of include guard: SEMPR_ENTITY_RDFVECTOR_H_ */
