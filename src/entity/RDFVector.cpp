#include <sempr/entity/RDFVector.hpp>
#include <RDFVector_odb.h>
#include <Soprano/Soprano>

namespace sempr { namespace entity {

// iterator impl
RDFVectorIterator_impl::RDFVectorIterator_impl(std::vector<Triple>::const_iterator it)
    : vit_(it)
{
}

RDFVectorIterator_impl::~RDFVectorIterator_impl()
{
}

const Triple& RDFVectorIterator_impl::operator *() const
{
    return *vit_;
}

const Triple* RDFVectorIterator_impl::operator->() const
{
    return vit_.operator->();
}

void RDFVectorIterator_impl::operator++()
{
    ++vit_;
}

bool RDFVectorIterator_impl::operator==(const TripleIterator_impl& other) const
{
    const RDFVectorIterator_impl* o = dynamic_cast<const RDFVectorIterator_impl*>(&other);
    if (!o) return false;
    return o->vit_ == this->vit_;
}
// ---------

SEMPR_ENTITY_SOURCE(RDFVector)

RDFVector::RDFVector(const core::IDGenBase* idgen)
    : RDFEntity(idgen)
{
    setDiscriminator<RDFVector>();
}


RDFVector::RDFVector() : RDFVector(new core::IDGen<RDFVector>())
{
}

// std::vector<Triple>::const_iterator RDFVector::begin() const
// {
//     return triples_.begin();
// }
//
// std::vector<Triple>::const_iterator RDFVector::end() const
// {
//     return triples_.end();
// }


void RDFVector::getTriples(std::vector<Triple> &triples) const
{
    triples.insert(triples.end(), triples_.begin(), triples_.end());
}

Triple& RDFVector::getTripleAt(const size_t &index)
{
    return triples_[index];
}

bool RDFVector::addTriple(const sempr::entity::Triple &triple)
{
    // check if the triple is valid!
    auto sub = Soprano::Node::fromN3(QString::fromStdString(triple.subject));
    auto pred = Soprano::Node::fromN3(QString::fromStdString(triple.predicate));
    auto obj = Soprano::Node::fromN3(QString::fromStdString(triple.object));

    Soprano::Statement st(sub, pred, obj);

    // but add anyway, necessary for RDFPropertyMap etc
    triples_.push_back(triple);
    this->changed();

    return st.isValid();
}

bool RDFVector::removeTriple(const sempr::entity::Triple &triple)
{
    auto newEnd = std::remove(triples_.begin(), triples_.end(), triple);
    if (newEnd == triples_.end()) return false;

    triples_.erase(newEnd, triples_.end());
    this->changed();
    return true;
}

void RDFVector::removeTripleAt(const size_t& index) {
    triples_.erase(triples_.begin() + index);
}

void RDFVector::clear()
{
    triples_.clear();
    this->changed();
}

size_t RDFVector::size() const {
    return triples_.size();
}

TripleIterator RDFVector::begin() const
{
    return TripleIterator(new RDFVectorIterator_impl(triples_.begin()));
}

TripleIterator RDFVector::end() const
{
    return TripleIterator(new RDFVectorIterator_impl(triples_.end()));
}


} /* entity */
} /* sempr */
