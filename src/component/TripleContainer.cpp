#include "component/TripleContainer.hpp"

namespace sempr {


Triple::Triple( const std::string& s,
                const std::string& p,
                const std::string& o )
    : subject_(s), predicate_(p), object_(o)
{
}


const std::string& Triple::getField(Triple::Field field) const
{
    switch (field) {
        case Triple::Field::SUBJECT:
            return subject_;
        case Triple::Field::PREDICATE:
            return predicate_;
        case Triple::Field::OBJECT:
            return object_;
    };
    throw std::exception(); // cannot happen, all cases for field are covered.
    // but the compiler doesn't realize that and issues a warning...
}

void Triple::setField(Triple::Field field, const std::string& value)
{
    switch (field) {
        case Triple::Field::SUBJECT:
            subject_ = value;
            break;
        case Triple::Field::PREDICATE:
            predicate_ = value;
            break;
        case Triple::Field::OBJECT:
            object_ = value;
            break;
    };
}

std::string Triple::toString() const
{
    return "(" + subject_ + " " + predicate_ + " " + object_ + ")";
}

bool Triple::operator==(const Triple& other) const
{
    return subject_ == other.subject_ &&
           predicate_ == other.predicate_ &&
           object_ == other.object_;
}

std::string Triple::fieldName(Triple::Field field)
{
    return rete::Triple::fieldName(field);
}

// -----------------------------------------------------------------------------

TripleIteratorImpl::~TripleIteratorImpl()
{
}

TripleIterator::TripleIterator(TripleIteratorImpl* impl)
    : impl_(impl)
{
    if (!impl_) throw std::exception();
}

TripleIterator::~TripleIterator()
{
    delete impl_;
}

const Triple TripleIterator::operator*() const
{
    return *(*impl_);
}

TripleIterator& TripleIterator::operator++()
{
    ++(*impl_);
    return *this;
}

bool TripleIterator::operator==(const TripleIterator& other) const
{
    return *impl_ == *(other.impl_);
}

bool TripleIterator::operator!=(const TripleIterator& other) const
{
    return !(*this == other);
}


void TripleContainer::getTriples(std::vector<Triple>& triples) const
{
    for (auto& t : *this)
    {
        triples.push_back(t);
    }
}

}
