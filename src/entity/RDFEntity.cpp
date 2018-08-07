#include <sempr/entity/RDFEntity.hpp>
#include <RDFEntity_odb.h>

namespace sempr { namespace entity {


// default implementation of the TripleIteratorWrapper: Does not return a triple, and every iterator equals another!
TripleIteratorWrapper::TripleIteratorWrapper(TripleIterator* impl)
    : impl_(impl)
{
}

TripleIteratorWrapper::~TripleIteratorWrapper()
{
    delete impl_;
}

const Triple TripleIteratorWrapper::operator*() const
{
    if (!impl_)
        throw std::exception();
    return *(*impl_);
}

TripleIteratorWrapper& TripleIteratorWrapper::operator++()
{
    if (!impl_) throw std::exception();
    ++(*impl_);
    return *this;
}

bool TripleIteratorWrapper::operator==(const sempr::entity::TripleIteratorWrapper &other) const
{
    if (!impl_) throw std::exception();
    return *impl_ == *(other.impl_);
}

bool TripleIteratorWrapper::operator!=(const TripleIteratorWrapper& other) const
{
    return !(*this == other);
}

// base impl
TripleIterator::~TripleIterator()
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

TripleIteratorWrapper RDFEntity::begin() const
{
    return TripleIteratorWrapper(nullptr);
}

TripleIteratorWrapper RDFEntity::end() const
{
    return TripleIteratorWrapper(nullptr);
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
