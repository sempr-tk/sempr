#include <sempr/entity/RDFVector.hpp>
#include <RDFVector_odb.h>
#include <Soprano/Soprano>

namespace sempr { namespace entity {

// iterator impl
RDFVectorIterator::RDFVectorIterator(std::vector<Triple>::const_iterator it)
    : vit_(it)
{
}

RDFVectorIterator::~RDFVectorIterator()
{
}

const Triple RDFVectorIterator::operator *() const
{
    return *vit_;
}

void RDFVectorIterator::operator++()
{
    ++vit_;
}

bool RDFVectorIterator::operator==(const TripleIterator& other) const
{
    const RDFVectorIterator* o = dynamic_cast<const RDFVectorIterator*>(&other);
    if (!o) return false;
    return o->vit_ == this->vit_;
}
// ---------

SEMPR_ENTITY_SOURCE(RDFVector)

RDFVector::RDFVector(const core::IDGenBase* idgen, bool temporary) : 
    RDFEntity(idgen, temporary)
{
    setDiscriminator<RDFVector>();
}

RDFVector::RDFVector(bool temporary) : 
    RDFVector(new core::IDGen<RDFVector>(), temporary)
{
}

RDFVector::RDFVector() : 
    RDFVector(new core::IDGen<RDFVector>())
{
}


void RDFVector::getTriples(std::vector<Triple> &triples) const
{
    triples.insert(triples.end(), triples_.begin(), triples_.end());
}

const Triple& RDFVector::getTripleAt(const size_t index)
{
    return triples_[index];
}

bool RDFVector::addTriple(const Triple& triple, bool replace)
{
    bool isValid = validity(triple);

    if (isValid)
    {
        if (replace)
            removeTriple(triple);

        triples_.push_back(triple);
        //this->changed();
    }

    return isValid;
}

bool RDFVector::validity(const Triple& triple) const
{
    Triple t = triple;
    // check if the triple is valid!
    auto sub = Soprano::Node::fromN3(QString::fromStdString(t.subject));
    auto pred = Soprano::Node::fromN3(QString::fromStdString(t.predicate));
    auto obj = Soprano::Node::fromN3(QString::fromStdString(t.object));

    Soprano::Statement st(sub, pred, obj);

    return st.isValid();
}

bool RDFVector::removeTriple(const Triple& triple)
{
    auto newEnd = std::remove(triples_.begin(), triples_.end(), triple);
    if (newEnd == triples_.end()) return false;

    triples_.erase(newEnd, triples_.end());
    //this->changed();
    return true;
}

void RDFVector::removeTripleAt(const size_t index) {
    triples_.erase(triples_.begin() + index);
    //this->changed();
}

void RDFVector::clear()
{
    triples_.clear();
    //this->changed();
}

size_t RDFVector::size() const {
    return triples_.size();
}

TripleIteratorWrapper RDFVector::begin() const
{
    return TripleIteratorWrapper(new RDFVectorIterator(triples_.begin()));
}

TripleIteratorWrapper RDFVector::end() const
{
    return TripleIteratorWrapper(new RDFVectorIterator(triples_.end()));
}


} /* entity */
} /* sempr */
