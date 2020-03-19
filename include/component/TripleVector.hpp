#ifndef SEMPR_COMPONENT_TRIPLEVECTOR_HPP_
#define SEMPR_COMPONENT_TRIPLEVECTOR_HPP_

#include <cereal/types/vector.hpp>

#include "TripleContainer.hpp"

namespace sempr {

/**
    The TripleVector is basically a wrapper around an std::vector<Triple>.
    It does not check for duplicates but simply stores and returns what it is
    given.
*/
class TripleVector : public TripleContainer {
    std::vector<Triple> triples_;
public:
    using Ptr = std::shared_ptr<TripleVector>;

    /**
        Appends the triples to the given std::vector
    */
    void getTriples(std::vector<Triple>& triples) const;

    /**
        Returns the triple at the given index (read only)
    */
    const Triple& getTripleAt(const size_t index) const;

    /**
        Adds a triple to the vector
    */
    void addTriple(const Triple& triple);

    /**
        Removes the first found instance of the given triple.
        Returns true if a matching triple was found, false otherwise.
    */
    bool removeTriple(const Triple& triple);

    /**
        Removes the triple at the given index
    */
    void removeTripleAt(const size_t index);

    /**
        Removes all triples
    */
    void clear();

    /**
        Returns the number of stored triples
    */
    size_t size() const;


    /**
        Allow iterating the TripleVector
    */
    TripleIterator begin() const;
    TripleIterator end() const;

    /**
        Serialization with cereal
    */
    template <class Archive>
    void save(Archive& ar) const
    {
        ar( cereal::make_nvp<Archive>("base", cereal::base_class<Component>(this)),
            cereal::make_nvp<Archive>("triples", triples_) );
    }

    template <class Archive>
    void load(Archive& ar)
    {
        ar( cereal::make_nvp<Archive>("base", cereal::base_class<Component>(this)),
            cereal::make_nvp<Archive>("triples", triples_) );
    }
};




/**
    An TripleIteratorImpl implementation for std::vector<Triple>
*/
class TripleVectorIterator : public TripleIteratorImpl {
    std::vector<Triple>::const_iterator vit_;
public:
    TripleVectorIterator(std::vector<Triple>::const_iterator it);
    ~TripleVectorIterator();

    const Triple operator * () const override;
    void operator ++ () override;
    bool operator == (const TripleIteratorImpl& other) const override;
};

}

CEREAL_REGISTER_TYPE(sempr::TripleVector)


// also, register a to_string function to display TripleContainer instances in
// the rete network
#include <rete-core/Util.hpp>
namespace rete { namespace util {

template <> std::string to_string(const sempr::TripleVector& c);
template <> std::string to_string(const sempr::TripleVector::Ptr& c);

}}



#endif /* include guard: SEMPR_COMPONENT_TRIPLEVECTOR_HPP_ */
