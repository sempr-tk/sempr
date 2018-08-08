#ifndef SEMPR_ENTITY_RDFENTITY_HPP_
#define SEMPR_ENTITY_RDFENTITY_HPP_

#include <sempr/core/RDF.hpp>
#include <sempr/entity/Entity.hpp>
#include <sempr/entity/Triple.hpp>

#include <sempr/core/EntityEvent.hpp>

namespace sempr { namespace entity {


class TripleIterator {
    friend class TripleIteratorWrapper;
protected:
    virtual ~TripleIterator();

    virtual const Triple operator * () const = 0;
    virtual void operator ++ () = 0;
    virtual bool operator == (const TripleIterator& other) const = 0;
};

/**
    Custom abstract iterator to hide implementation of the container.
    This way a RDFEntity can use a vector or a map or whatever it likes, just by implementing
    the TripleIterator_impl.

    This class uses the pimpl-idiom: Every RDFEntity-subtype returns a TripleIterator, but with
    different implementations. To create your own implement a subclass of TripleIterator_impl and
    pass an instance of it to the ctor of the TripleIterator.

    Although the class is called "[...]Iterator" it not really behaves like one.
    Since we cannot assert that every container implementing this actually stores triples which
    we could reference, it  needs to return triples by value instead.
*/
class TripleIteratorWrapper {
    TripleIterator* const impl_;
public:
    TripleIteratorWrapper(TripleIterator* impl);
    ~TripleIteratorWrapper();

    /**
        "read-only" access: Creates a copy of the represented triple.
     */
    const Triple operator * () const;

    TripleIteratorWrapper& operator ++ ();
    bool operator == (const TripleIteratorWrapper& other) const;
    bool operator != (const TripleIteratorWrapper& other) const;
};


/**
    The RDFEntity is the interface for all entities that focus on providing rdf triples. It only
    allows iteration over the (constant!) triples, nothing more. This way every module may read the
    triples, but only modify them if they know how to exactly. (This was a major problem before,
    when RDFPropertyMap inherited RDFVector, so through RDFVector the RDFPropertyMap could become
    invalid!)
*/
#pragma db object
class RDFEntity : public Entity {
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<RDFEntity>;
    virtual ~RDFEntity(){};

    /** copy the triples into a vector */
    void getTriples(std::vector<Triple>& triples) const;

    /** iterate the triples */
    virtual TripleIteratorWrapper begin() const;
    virtual TripleIteratorWrapper end() const;

protected:
    RDFEntity(const core::IDGenBase*);
private:
    friend class odb::access;
    RDFEntity();
};


} /* entity */

} /* sempr */

#endif /* end of include guard: SEMPR_ENTITY_RDFENTITY_HPP_ */
