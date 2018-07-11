#ifndef SEMPR_ENTITY_RDFENTITY_HPP_
#define SEMPR_ENTITY_RDFENTITY_HPP_

#include <sempr/core/RDF.hpp>
#include <sempr/entity/Entity.hpp>
#include <sempr/entity/Triple.hpp>

#include <sempr/core/EntityEvent.hpp>

namespace sempr { namespace entity {


class TripleIterator_impl {
    friend class TripleIterator;
protected:
    virtual ~TripleIterator_impl();

    virtual const Triple& operator * () const = 0;
    virtual const Triple* operator -> () const = 0;
    virtual void operator ++ () = 0;
    virtual bool operator == (const TripleIterator_impl& other) const = 0;
};

/**
    Custom abstract iterator to hide implementation of the container.
    This way a RDFEntity can use a vector or a map or whatever it likes, just by implementing
    the TripleIterator_impl.

    This class uses the pimpl-idiom: Every RDFEntity-subtype returns a TripleIterator, but with
    different implementations. To create your own implement a subclass of TripleIterator_impl and
    pass an instance of it to the ctor of the TripleIterator.
*/
class TripleIterator {
    TripleIterator_impl* impl_;
public:
    TripleIterator(TripleIterator_impl* impl);
    ~TripleIterator();
    const Triple& operator * () const;
    const Triple* operator -> () const;
    TripleIterator& operator ++ ();
    bool operator == (const TripleIterator& other) const;
    bool operator != (const TripleIterator& other) const;
};


/**
    The RDFEntity is the interface for all entities that focus on providing rdf triples. It only
    allows iteration over the (constant!) triples, nothing more. This way every module may read the
    triples, but only modify them if they know how to exactly. (This was a major problem before,
    when RDFPropertyMap inherited RDFVector, so through RDFVector the RDFPropertyMap could become
    invalid!) // TODO -- this is still the case!
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
    virtual TripleIterator begin() const;
    virtual TripleIterator end() const;

protected:
    RDFEntity(const core::IDGenBase*);
private:
    friend class odb::access;
    RDFEntity();
};


} /* entity */

} /* sempr */

#endif /* end of include guard: SEMPR_ENTITY_RDFENTITY_HPP_ */
