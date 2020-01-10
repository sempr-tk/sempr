#ifndef SEMPR_CORE_COMPONENT_TRIPLECONTAINER_HPP_
#define SEMPR_CORE_COMPONENT_TRIPLECONTAINER_HPP_

#include <vector>
#include <string>

#include <rete-rdf/Triple.hpp>

#include "Component.hpp"
#include "Utility.hpp"

namespace sempr {

/**
    A custom Triple-implementation for sempr.

    I first tried to use rete::Triple instead, but this leads to some problems:
    rete::Triple inherits rete::WME, so using it as a component is a bit
    awkward. Also, it is immutable: Subject, predicate and object are constant
    strings. This prevents any manipulation after construction them, which
    includes copy assignment! But copy (or move) assignment is necessary when
    working with triples in a stl container. E.g., std::remove works by moving
    the to-be-deleted entries to the end of a vector -- which is not possible
    without a copy assignment operator. Furthermore, in sempr we might want to
    edit triples, and then just infer the immutable rete::Triples.
*/
class Triple {
    std::string subject_, predicate_, object_;
public:
    using Ptr = std::shared_ptr<Triple>;

    Triple( const std::string& s,
            const std::string& p,
            const std::string& o );

    // a little bit of compatibility to rete::Triple
    using Field = rete::Triple::Field;

    /**
        Get the value stored in either Triple::SUBJECT, PREDICATE or OBJECT
    */
    const std::string& getField(Field) const;

    /**
        Set the value of Triple::SUBJECT, PREDICATE or OBJECT
    */
    void setField(Field, const std::string&);

    /**
        A string representation of the triple
    */
    std::string toString() const;

    /**
        Comparison operator
    */
    bool operator == (const Triple& other) const;

    static std::string fieldName(Field);
};


/**
    In order to provide a common interface for all triple-holding components we
    provide an abstract "TripleIteratorImpl" interface. This must be implemented
    by the different TripleContainer implementations to allow everyone to
    iterate over the contained triples.

    This approach follows the PIMPL idiom to allow returning a plain
    TripleIterator (no pointer, no reference) from the TripleContainer interface
    while still allowing specific implementations in the TripleIteratorImpl.
*/
class TripleIteratorImpl {
    friend class TripleIterator;
protected:
    virtual ~TripleIteratorImpl();

    virtual const Triple operator * () const = 0;
    virtual void operator ++ () = 0;
    virtual bool operator == (const TripleIteratorImpl& other) const = 0;
};


/**
    TripleIterator that delegates to a TripleIteratorImpl.
*/
class TripleIterator {
    TripleIteratorImpl* const impl_;
public:
    TripleIterator(TripleIteratorImpl* impl);
    ~TripleIterator();

    /**
        The iterator returns triples by value, not by reference, as some
        containers might only contain virtual triples and instantiate them from
        other values only when needed.
    */
    const Triple operator * () const;

    TripleIterator& operator ++ ();
    bool operator == (const TripleIterator& other) const;
    bool operator != (const TripleIterator& other) const;
};



/**
    The TripleContainer interface
*/
class TripleContainer : public Component {
public:
    using Ptr = std::shared_ptr<TripleContainer>;

    /** copy the triples into a vector */
    void getTriples(std::vector<Triple>& triples) const;

    /** iterate the triples */
    virtual TripleIterator begin() const = 0;
    virtual TripleIterator end() const = 0;
};

// register component name
template <>
struct ComponentName<TripleContainer> {
    static constexpr const char* value = "TripleContainer";
};

};

#endif /* include guard: SEMPR_CORE_COMPONENT_TRIPLECONTAINER_HPP_ */
