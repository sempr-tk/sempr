#include <sempr/entity/RDFValue.hpp>


// custom specialization of conversion to std::string.
// QVariant/Soprano::LiteralValue won't allow it directly.
template <>
std::string sempr::entity::RDFValue::to(void*) const {
    return this->literal_.toString().toStdString();
}

// custom specialization for assignment of strings
template <>
sempr::entity::RDFValue& sempr::entity::RDFValue::operator = (const std::string& value) {
    type_ = Type::LITERAL;
    literal_ = QString::fromStdString(value);
    stringRepresentation_ = ("\"" + literal_.toString() + "\"^^<" + literal_.dataTypeUri().toString() + ">").toStdString();
}

sempr::entity::RDFValue& sempr::entity::RDFValue::operator = (const char* value) {
    type_ = Type::LITERAL;
    literal_ = QString(value);
    // stringRepresentation_ = ("\"" + literal_.toString() + "\"^^<" + literal_.dataTypeUri().toString() + ">").toStdString();
    Soprano::Node tmp = Soprano::Node::createLiteralNode(literal_);
    stringRepresentation_ = tmp.toN3().toStdString();
}


void sempr::entity::RDFValue::makeValid()
{
    if (valid_) return;

    node_ = Soprano::Node::fromN3(QString::fromStdString(stringRepresentation_));
    if (type_ == Type::LITERAL) {
        if (!node_.isLiteral())
            throw std::runtime_error("RDFValue makeValid() failed: not a literal");

        literal_ = node_.literal();
        std::cout << "makeValid. " << stringRepresentation_  << '\n';
        std::cout << "makeValid. " << node_.literal().toString().toStdString() << '\n';
    }

    valid_ = true;
}
