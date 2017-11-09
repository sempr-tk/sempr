#include <sempr/entity/RDFEntity.hpp>
#include <RDFEntity_odb.h>

namespace sempr { namespace entity {

RDFEntity::RDFEntity()
{
    setDiscriminator<RDFEntity>();
}


void RDFEntity::getTriples(std::vector<Triple> &triples) const
{
    triples.insert(triples.end(), triples_.begin(), triples_.end());
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

void RDFEntity::clear()
{
    triples_.clear();
    this->changed();
}


} /* entity */
} /* sempr */
