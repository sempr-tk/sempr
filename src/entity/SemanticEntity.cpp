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
SemanticEntityIterator_impl::SemanticEntityIterator_impl(SemanticEntityIterator_impl::ConstIterator it, SemanticEntityIterator_impl::ConstIterator end, const SemanticEntity* entity)
    : vit_(it), end_(end), entity_(entity)
{
    makeValid();
}

const Triple SemanticEntityIterator_impl::operator*() const
{
    Triple t;

    if ((*vit_)->hasSubject())
        t.subject = (*vit_)->subject();
    else t.subject = "<" + sempr::baseURI() + this->entity_->id() + ">";

    t.predicate = (*vit_)->predicate();
    t.object = (*vit_)->object();

    return t;
}


void SemanticEntityIterator_impl::operator++()
{
    ++vit_;
    // since we may be at an invalid entry now, make sure we are at end or valid!
    makeValid();
}

bool SemanticEntityIterator_impl::operator==(const sempr::entity::TripleIterator_impl &other) const
{
    auto o = dynamic_cast<const SemanticEntityIterator_impl*>(&other);
    if (!o) return false;
    return this->vit_ == o->vit_;
}

void SemanticEntityIterator_impl::makeValid()
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


TripleIterator SemanticEntity::begin() const
{
    return TripleIterator(new SemanticEntityIterator_impl(
        this->properties_.begin(), this->properties_.end(), this)
    );
}

TripleIterator SemanticEntity::end() const
{
    return TripleIterator(new SemanticEntityIterator_impl(
        this->properties_.end(), this->properties_.end(),  this)
    );
}


} /* entity */
} /* sempr */
