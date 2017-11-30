#ifndef SEMPR_ENTITY_RDFVALUE_HPP_
#define SEMPR_ENTITY_RDFVALUE_HPP_

#include <memory>
#include <type_traits>
#include <Soprano/Soprano>
#include <odb/core.hxx>
#include <sempr/entity/Entity.hpp>



namespace sempr { namespace entity {
// custom type traits needed in the implementation of RDFValue to differ
// between shared_ptr<T> and T
// Note: if defined outside the sempr::entity namespace this leads to very
// unambigous, non-meaningful error messages. Something along the lines:
//      error: wrong number of template arguments
//         [...]
//      access::composite_value_traits< ..., ... > 1, should be 2
namespace {
    template <typename T>
    struct remove_shared_ptr {
        typedef T type;
    };

    template <typename T>
    struct remove_shared_ptr<std::shared_ptr<T> > {
        typedef T type;
    };

    template <typename T>
    struct is_shared_ptr {
        static constexpr bool value = false;
    };

    template <typename T>
    struct is_shared_ptr<std::shared_ptr<T> > {
        static constexpr bool value = true;
    };
}

/**
    A container type to take either a literal value, a pointer to another
    Entity or a resource URI. It allows implicit conversions and assignment
    of values. It is used to represent the object part of an RDF-Triple in
    the RDFPropertyMap and allows an easys c++-interface:
    ´´´
        RDFValue c;
        c = 123;
        c = 12.3f;
        c = "Hello World!";
        std::string tmp = c;
        SomeEntity::Ptr e(new SomeEntity());
        c = e;
        //[...]
    ´´´
    Note: Parsing an RDFValue from N3-String (e.g. "123"^^<http://[...]/#int>)
    is not implemented: The container itself is stored in the database, no need
    to parse. (Also, how would you get the Entity::Ptr from an URI?).
*/
#pragma db value callback(init)
class RDFValue {
private:
    friend class odb::access;
    enum Type { LITERAL, RESOURCE, POINTER };
    int type_;
    Entity::Ptr pointer_;
    std::string stringRepresentation_;

    #pragma db transient
    Soprano::LiteralValue literal_;
    #pragma db transient
    Soprano::Node node_;
    #pragma db transient
    bool valid_; // helper: needed to restore literal_ or node_
                // from stringRepresentation_


    // Conversions:
    // if we don't cast to a shared_ptr it must be a literal.
    /** Cast to literal (int, float, ...) */
    template <typename T>
    T to(typename std::enable_if<not is_shared_ptr<T>::value, void*>::type = 0) const {
        if (type_ == Type::LITERAL) {
            return literal_.variant().value<T>();
        }
        throw std::runtime_error(std::string("RDFValue is not literal, tried to cast to ") + typeid(T).name());
    }


    /** Cast to shared-ptr */
    template <typename T>
    T to(typename std::enable_if<is_shared_ptr<T>::value, void*>::type = 0) const {
        if (type_ == Type::POINTER) {
            // return std::static_pointer_cast<typename remove_shared_ptr<T>::type>(pointer_);
            T tmp = std::dynamic_pointer_cast<typename remove_shared_ptr<T>::type>(pointer_);
            if (tmp.get() == 0) {
                // invalid type or nullptr. how could there be a nullptr?
                // why would anyone store a nullptr?!
                throw std::runtime_error(std::string("RDFValue: dynamic_pointer_cast to ") + typeid(T).name() + " failed.");
            }
            return tmp;
        }
        throw std::runtime_error(std::string("RDFValue is not a pointer, tried to cast to ") + typeid(T).name());
    }

    // restore the internal, transient members after loading from the database.
    void makeValid();

public:
    RDFValue() : valid_(false), pointer_(NULL) {}

    /** Assign a literal value */
    template <typename T>
    RDFValue& operator = (const T& other) {
        type_ = Type::LITERAL;
        literal_ = other;
        // stringRepresentation_ = ("\"" + literal_.toString() + "\"^^<" + literal_.dataTypeUri().toString() + ">").toStdString();
        Soprano::Node tmp = Soprano::Node::createLiteralNode(literal_);
        stringRepresentation_ = tmp.toN3().toStdString();
        return *this;
    }

    /** Assign a pointer to a Entity */
    template <typename T>
    RDFValue& operator = (const std::shared_ptr<T>& other) {
        static_assert(std::is_base_of<Entity, T>::value, "Type not derived from Entity");
        type_ = Type::POINTER;
        pointer_ = other;
        // TODO: need a central point of namespace definitions
        // stringRepresentation_ = ("<" + std::string("sempr://") + pointer_->id() + ">");
        Soprano::Node tmp = Soprano::Node::createResourceNode(
            QUrl(QString("sempr://") + QString::fromStdString(pointer_->id()))
        );
        stringRepresentation_ = tmp.toN3().toStdString();
        return *this;
    }

    // overload for c-strings (so that map["name"]="Max" wont result in "true"^^<xsd:boolean>)
    RDFValue& operator = (const char* cstr);

    /** Casts. Delegates to "to<T>()" since the operators syntax doesn't allow
        for enable_if<...> - differentiation
    */
    template <typename T>
    operator T() {
        makeValid();
        return to<T>();
    }

    /** Just another way to cast. */
    template <typename T>
    T get() const {
        return *this;
    }

    /** return the string representation of the value */
    std::string toString() const { return stringRepresentation_; }

};


// prevent implicit specialization for string case, provide our own
template <> std::string RDFValue::to(void*) const;
// defined in source, to prevent multiple-definition-error
// (compiler treats it as an ordinary function...)

// same for assignment of string. QStrings work, but std::string is not supported
// (compiler error), and const char* is mapped to boolean!
template <> RDFValue& RDFValue::operator = (const std::string& value);
// need to overwrite the const char* variant, too! else map["name"] = "Max" still
// results in "true"^^<xsd:boolean>
// template <> RDFValue& RDFValue::operator = (const char& value);
// done via overloading inside class.

}}

#endif /* end of include guard: SEMPR_ENTITY_RDFVALUE_HPP_ */
