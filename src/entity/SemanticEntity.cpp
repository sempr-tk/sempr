#include <sempr/entity/SemanticEntity.hpp>
#include <SemanticEntity_odb.h>

namespace sempr { namespace entity {

/// RegisteredProperty
RegisteredPropertyBase::RegisteredPropertyBase(const std::string& predicate)
    : hasSubject_(false), predicate_(predicate)
{
}

RegisteredPropertyBase::RegisteredPropertyBase(const std::string& sub, const std::string& pred)
    : hasSubject_(true), subject_(sub), predicate_(pred)
{
}

bool RegisteredPropertyBase::hasSubject() const
{
    return hasSubject_;
}

std::string RegisteredPropertyBase::subject() const
{
    return subject_;
}

std::string RegisteredPropertyBase::predicate() const
{
    return predicate_;
}


/// SemanticEntity iterator
SemanticEntityIterator::SemanticEntityIterator(SemanticEntityIterator::ConstIterator it, SemanticEntityIterator::ConstIterator end, const SemanticEntity* entity)
    : vit_(it), end_(end), entity_(entity)
{
    makeValid();
}

const Triple SemanticEntityIterator::operator*() const
{
    Triple t;

    if ((*vit_)->hasSubject())
        t.subject = (*vit_)->subject();
    else t.subject = "<" + sempr::baseURI() + this->entity_->id() + ">";

    t.predicate = (*vit_)->predicate();
    t.object = (*vit_)->object();

    return t;
}


void SemanticEntityIterator::operator++()
{
    ++vit_;
    // since we may be at an invalid entry now, make sure we are at end or valid!
    makeValid();
}

bool SemanticEntityIterator::operator==(const sempr::entity::TripleIterator &other) const
{
    auto o = dynamic_cast<const SemanticEntityIterator*>(&other);
    if (!o) return false;
    return this->vit_ == o->vit_;
}

void SemanticEntityIterator::makeValid()
{
    while (vit_ != end_ &&      // as long as we are not at the end
           !(*vit_)->isValid()) // and the current entry is not valid
    {
        ++vit_;                 // proceed to next entry.
    }
}


/// SemanticEntity
SEMPR_ENTITY_SOURCE(SemanticEntity);

SemanticEntity::SemanticEntity(const core::IDGenBase* idgen)
    : RDFEntity(idgen)
{
    this->setDiscriminator<SemanticEntity>();
}

SemanticEntity::SemanticEntity()
    : SemanticEntity(new core::IDGen<SemanticEntity>())
{
}

SemanticEntity::~SemanticEntity()
{
    for (auto rprop : properties_)
    {
        delete rprop;
    }
}


TripleIteratorWrapper SemanticEntity::begin() const
{
    return TripleIteratorWrapper(new SemanticEntityIterator(
        this->properties_.begin(), this->properties_.end(), this)
    );
}

TripleIteratorWrapper SemanticEntity::end() const
{
    return TripleIteratorWrapper(new SemanticEntityIterator(
        this->properties_.end(), this->properties_.end(),  this)
    );
}


} /* entity */
} /* sempr */
