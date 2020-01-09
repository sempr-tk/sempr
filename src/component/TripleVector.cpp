#include <algorithm>

#include "component/TripleVector.hpp"

namespace sempr {

void TripleVector::getTriples(std::vector<Triple>& triples) const
{
    triples.insert(triples.end(), triples_.begin(), triples_.end());
}

const Triple& TripleVector::getTripleAt(const size_t index) const
{
    return triples_[index];
}

void TripleVector::addTriple(const Triple& triple)
{
    // TODO: check if the triple is valid?
    // actually.. no.. I renamed this from "RDFVector" to "TripleVector", which
    // does not necessarily impose such constraints on the values.

    triples_.push_back(triple);
}

bool TripleVector::removeTriple(const Triple& triple)
{
    auto newEnd = std::remove(triples_.begin(), triples_.end(), triple);
    if (newEnd == triples_.end()) return false;

    triples_.erase(newEnd, triples_.end());
    return true;
}

void TripleVector::removeTripleAt(const size_t index)
{
    triples_.erase(triples_.begin() + index);
}

void TripleVector::clear()
{
    triples_.clear();
}

size_t TripleVector::size() const
{
    return triples_.size();
}

TripleIterator TripleVector::begin() const
{
    return TripleIterator(new TripleVectorIterator(triples_.begin()));
}


TripleIterator TripleVector::end() const
{
    return TripleIterator(new TripleVectorIterator(triples_.begin()));
}


// ------------------------------------------

TripleVectorIterator::TripleVectorIterator(std::vector<Triple>::const_iterator it)
    : vit_(it)
{
}

TripleVectorIterator::~TripleVectorIterator()
{
}

const Triple TripleVectorIterator::operator * () const
{
    return *vit_;
}

void TripleVectorIterator::operator ++ ()
{
    ++ vit_;
}

bool TripleVectorIterator::operator == (const TripleIteratorImpl& other) const
{
    const TripleVectorIterator* o = dynamic_cast<const TripleVectorIterator*>(&other);
    if (!o) return false; // or rather, throw an exception?!
    return o->vit_ == this->vit_;
}



}
