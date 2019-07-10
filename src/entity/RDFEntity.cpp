#include <sempr/entity/RDFEntity.hpp>
#include <RDFEntity_odb.h>

namespace sempr { namespace entity {


TripleIteratorWrapper::TripleIteratorWrapper(TripleIterator* impl)
    : impl_(impl)
{
    if (!impl_) throw std::exception();
}

TripleIteratorWrapper::~TripleIteratorWrapper()
{
    delete impl_;
}

const Triple TripleIteratorWrapper::operator*() const
{
    return *(*impl_);
}

TripleIteratorWrapper& TripleIteratorWrapper::operator++()
{
    ++(*impl_);
    return *this;
}

bool TripleIteratorWrapper::operator==(const sempr::entity::TripleIteratorWrapper &other) const
{
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


RDFEntity::RDFEntity(const core::IDGenBase* idgen, bool temporary)
    : Entity(idgen, temporary)
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
