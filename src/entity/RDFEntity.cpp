#include <sempr/entity/RDFEntity.hpp>
#include <RDFEntity_odb.h>

namespace sempr { namespace entity {

RDFEntity::RDFEntity(const core::IDGenBase& idgen)
    : Entity(idgen)
{
    setDiscriminator<RDFEntity>();
}

RDFEntity::RDFEntity() : RDFEntity(core::IDGen<RDFEntity>())
{
}


void RDFEntity::getTriples(std::vector<Triple> &triples) const
{
    triples.insert(triples.end(), triples_.begin(), triples_.end());
}

Triple& RDFEntity::getTripleAt(const size_t &index)
{
    return triples_[index];
}

void RDFEntity::addTriple(const sempr::entity::Triple &triple)
{
    triples_.push_back(triple);
    this->changed();
}

void RDFEntity::removeTriple(const sempr::entity::Triple &triple)
{
    auto newEnd = std::remove(triples_.begin(), triples_.end(), triple);
    triples_.erase(newEnd);
    this->changed();
}

void RDFEntity::removeTripleAt(const size_t& index) {
    triples_.erase(triples_.begin() + index);
}

void RDFEntity::clear()
{
    triples_.clear();
    this->changed();
}

size_t RDFEntity::size() const {
    return triples_.size();
}


} /* entity */
} /* sempr */
