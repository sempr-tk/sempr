#include <string>
#include <algorithm>

#include "component/TriplePropertyMap.hpp"
#include "Exception.hpp"
#include "Entity.hpp"
#include "RDF.hpp"

namespace sempr {

// --------------------------------------------------------------------------
// TriplePropertyMapEntry
// --------------------------------------------------------------------------
TriplePropertyMapEntry::TriplePropertyMapEntry()
    : type_(Type::INVALID), strValue_(""), fltValue_(0.f), intValue_(0)
{
}

TriplePropertyMapEntry::TriplePropertyMapEntry(const std::string& str, bool isResource)
    : strValue_(str), fltValue_(0.f), intValue_(0)
{
    type_ = (isResource ? Type::RESOURCE : Type::STRING);
}

TriplePropertyMapEntry::TriplePropertyMapEntry(const char* str, bool isResource)
    : strValue_(str), fltValue_(0.f), intValue_(0)
{
    type_ = (isResource ? Type::RESOURCE : Type::STRING);
}

TriplePropertyMapEntry::TriplePropertyMapEntry(float val)
    : type_(Type::FLOAT), strValue_(""), fltValue_(val), intValue_(0)
{
}

TriplePropertyMapEntry::TriplePropertyMapEntry(int val)
    : type_(Type::INT), strValue_(""), fltValue_(0.f), intValue_(val)
{
}

TriplePropertyMapEntry::Type TriplePropertyMapEntry::type() const
{
    return type_;
}

TriplePropertyMapEntry::operator std::string() const
{
    if (!(type_ == Type::STRING || type_ == Type::RESOURCE))
        throw sempr::Exception("TriplePropertyMapEntry is not a string or resource");
    return strValue_;
}

TriplePropertyMapEntry::operator float() const
{
    if (!(type_ == Type::FLOAT)) throw sempr::Exception("TriplePropertyMapEntry is not a float");
    return fltValue_;
}

TriplePropertyMapEntry::operator int() const
{
    if (!(type_ == Type::INT)) throw sempr::Exception("TriplePropertyMapEntry is not an int");
    return intValue_;
}


std::string TriplePropertyMapEntry::toN3() const
{
    std::string val;
    switch (type_) {
        case Type::RESOURCE:
        case Type::STRING:
            val = strValue_;
            break;
        case Type::FLOAT:
            val = std::to_string(fltValue_);
            break;
        case Type::INT:
            val = std::to_string(intValue_);
            break;
        case Type::INVALID:
            val = ""; // just an empty string, will not even be quoted and thus
                      // stays invalid rdf, right?
    }

    if (type_ == Type::STRING)
    {
        // escape quotation marks and backslashes
        auto replace = [](std::string& str, const std::string& from, const std::string& to) -> void
        {
            if (from.empty()) return;

            size_t start_pos = 0;
            while ((start_pos = str.find(from, start_pos)) != std::string::npos)
            {
                str.replace(start_pos, from.length(), to);
                start_pos += to.length();
            }
        };

        // first the backslashes
        replace(val, "\\", "\\\\");
        // then the quotes
        replace(val, "\"", "\\\"");

        // and wrap everything in quotes.
        val = "\"" + val + "\"";
    }
    else if (type_ == Type::RESOURCE)
    {
        // wrap everything in <>
        val = "<" + val + ">";
    }
    return val;
}

// --------------------------------------------------------------------------
// TriplePropertyMap
// --------------------------------------------------------------------------
TripleIterator TriplePropertyMap::begin() const
{
    std::string sub = "";
    if (this->getEntity())
    {
        sub = "<" + sempr::baseURI() + this->getEntity()->id() + ">";
    }

    return TripleIterator(
               TripleIteratorImpl::Ptr(
                   new TriplePropertyMapIterator(map_.begin(), sub)
               )
           );
}

TripleIterator TriplePropertyMap::end() const
{
    std::string sub = "";
    if (this->getEntity())
    {
        sub = "<" + sempr::baseURI() + this->getEntity()->id() + ">";
    }

    return TripleIterator(
               TripleIteratorImpl::Ptr(
                   new TriplePropertyMapIterator(map_.end(), sub)
               )
           );
}

// --------------------------------------------------------------------------
// TriplePropertyMapIterator
// --------------------------------------------------------------------------
TriplePropertyMapIterator::TriplePropertyMapIterator(
        std::map<std::string, TriplePropertyMapEntry>::const_iterator it,
        const std::string& subject)
    : it_(it), subject_(subject)
{
}

TriplePropertyMapIterator::~TriplePropertyMapIterator()
{
}

const Triple TriplePropertyMapIterator::operator * () const
{
    return Triple( subject_,
                   "<" + it_->first + ">",
                   it_->second.toN3() );
}

void TriplePropertyMapIterator::operator ++ ()
{
    ++it_;
}

bool TriplePropertyMapIterator::operator ==(const TripleIteratorImpl& other) const
{
    const TriplePropertyMapIterator* o = dynamic_cast<const TriplePropertyMapIterator*>(&other);
    if (!o) return false;
    return o->it_ == this->it_;
}


}
