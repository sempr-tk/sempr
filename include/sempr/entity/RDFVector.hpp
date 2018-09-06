#ifndef SEMPR_ENTITY_RDFVECTOR_H_
#define SEMPR_ENTITY_RDFVECTOR_H_

#include <odb/core.hxx>
#include <sempr/entity/RDFEntity.hpp>

#include <vector>

namespace sempr { namespace entity {

// custom triple-iterator for the vector class
class RDFVectorIterator : public TripleIterator {
    friend class RDFVector;
    std::vector<Triple>::const_iterator vit_;

    RDFVectorIterator(std::vector<Triple>::const_iterator it);
    ~RDFVectorIterator();

    const Triple operator * () const override;
    void operator ++ () override;
    bool operator == (const TripleIterator& other) const override;
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
    const Triple& getTripleAt(const size_t index);
    bool addTriple(const Triple& triple, bool replace = false);     // if replace than an equal triple will be removed before, otherwise there a multiple entries possible
    bool removeTriple(const Triple& triple);
    void removeTripleAt(const size_t index);
    void clear();
    size_t size() const;

    TripleIteratorWrapper begin() const override;
    TripleIteratorWrapper end() const override;

protected:
    friend class odb::access;
    std::vector<Triple> triples_;
};



}}

#endif /* end of include guard: SEMPR_ENTITY_RDFVECTOR_H_ */
