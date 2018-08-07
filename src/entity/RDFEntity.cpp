#include <sempr/entity/RDFEntity.hpp>
#include <RDFEntity_odb.h>

namespace sempr { namespace entity {


// default implementation of the TripleIterator: Does not return a triple, and every iterator equals another!
TripleIterator::TripleIterator(TripleIterator_impl* impl)
    : impl_(impl)
{
}

TripleIterator::~TripleIterator()
{
    delete impl_;
}

const Triple TripleIterator::operator*() const
{
    if (!impl_)
        throw std::exception();
    return *(*impl_);
}

TripleIterator& TripleIterator::operator++()
{
    if (!impl_) throw std::exception();
    ++(*impl_);
    return *this;
}

bool TripleIterator::operator==(const sempr::entity::TripleIterator &other) const
{
    if (!impl_) throw std::exception();
    return *impl_ == *(other.impl_);
}

bool TripleIterator::operator!=(const TripleIterator& other) const
{
    return !(*this == other);
}

// base impl
TripleIterator_impl::~TripleIterator_impl()
{
}





// ------

SEMPR_ENTITY_SOURCE(RDFEntity);


RDFEntity::RDFEntity(const core::IDGenBase* idgen)
    : Entity(idgen)
{
}


// private, only to be used by odb!
RDFEntity::RDFEntity()
    : Entity()
{
}

TripleIterator RDFEntity::begin() const
{
    return TripleIterator(nullptr);
}

TripleIterator RDFEntity::end() const
{
    return TripleIterator(nullptr);
}

void RDFEntity::getTriples(std::vector<Triple>& triples) const
{
    for (auto& t : *this)
    {
        triples.push_back(t);
    }
}


} /* entity */
} /* sempr */
