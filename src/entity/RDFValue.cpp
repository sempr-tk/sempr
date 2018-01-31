#include <sempr/entity/RDFValue.hpp>


// custom specialization of conversion to std::string.
// QVariant/Soprano::LiteralValue won't allow it directly.
// template <>
// std::string sempr::entity::RDFValue::to(void*) const {
//     return this->literal_.toString().toStdString();
// }

// if we cast to string we need to check if it is a resource node!
template <>
std::string sempr::entity::RDFValue::to(void*) const {
    if (type_ == Type::LITERAL) {
        return literal_.toString().toStdString();
    } else if (type_ == Type::RESOURCE) {
        return stringRepresentation_;
    }
    throw std::runtime_error(std::string("RDFValue is not a literal or resource, tried to cast to std::string."));
}

// custom specialization for assignment of strings
template <>
sempr::entity::RDFValue& sempr::entity::RDFValue::operator = (const std::string& value) {
    type_ = Type::LITERAL;
    literal_ = QString::fromStdString(value);
    stringRepresentation_ = ("\"" + literal_.toString() + "\"^^<" + literal_.dataTypeUri().toString() + ">").toStdString();
    return *this;
}


/** Assign a resource (e.g. rdf:type sempr:Person) */
template <>
sempr::entity::RDFValue& sempr::entity::RDFValue::operator = (const sempr::entity::RDFResource& other)
{
    type_ = Type::RESOURCE;
    // simply store the resource as a string. e.g. "<http://example.com#foo>"
    stringRepresentation_ = other.value_;
    return *this;
}



sempr::entity::RDFValue& sempr::entity::RDFValue::operator = (const char* value) {
    type_ = Type::LITERAL;
    literal_ = QString(value);
    // stringRepresentation_ = ("\"" + literal_.toString() + "\"^^<" + literal_.dataTypeUri().toString() + ">").toStdString();
    Soprano::Node tmp = Soprano::Node::createLiteralNode(literal_);
    stringRepresentation_ = tmp.toN3().toStdString();
    return *this;
}


void sempr::entity::RDFValue::makeValid()
{
    if (valid_) return;

    node_ = Soprano::Node::fromN3(QString::fromStdString(stringRepresentation_));
    if (type_ == Type::LITERAL) {
        if (!node_.isLiteral())
            throw std::runtime_error("RDFValue makeValid() failed: not a literal");

        literal_ = node_.literal();
        // std::cout << "makeValid. " << stringRepresentation_  << '\n';
        // std::cout << "makeValid. " << node_.literal().toString().toStdString() << '\n';
    }

    valid_ = true;
}
