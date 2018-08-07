#ifndef SEMPR_ENTITY_SEMANTICENTITY_HPP
#define SEMPR_ENTITY_SEMANTICENTITY_HPP

#include <odb/core.hxx>
#include <odb/wrapper-traits.hxx>
#include <sempr/entity/RDFEntity.hpp>
#include <memory>

#include <Soprano/Soprano> // for the LiteralValue conversion functions
#include <type_traits>
#include <sempr/core/RDF.hpp>

namespace sempr {

namespace rdf {
namespace traits {

    /**
        Traits to convert a value of a given type to a string that can be used in an RDF triple.
        Default implementation: Try to create a LiteralValue node from it. This works for simple
        ints and floats, but some standard types are not supported because Soprano relies on the qt
        definitions (e.g. qulonglong instead of unsigned long long or uint64_t).
    */
    template <typename T, typename Enable = void>
    struct n3_string
    {
        static std::string from(const T& value)
        {
            Soprano::Node tmp = Soprano::Node::createLiteralNode(value);
            return tmp.toN3().toStdString();
        }
    };

    /**
        Specialization for shared_ptrs to entities -- use their ID.
    */
    template <typename T>
    struct n3_string<
                std::shared_ptr<T>,
                typename std::enable_if<std::is_base_of<sempr::entity::Entity, T>::value>::type
            >
    {
        static std::string from(std::shared_ptr<T> value)
        {
            if (!value) return "";
            return "<" + sempr::baseURI() + value->id() + ">";
        }
    };

    /**
        Specialization for std::strings (which don't work out of the box, but QStrings should)
    */
    template <>
    struct n3_string<std::string, void>
    {
        static std::string from(const std::string& str)
        {
            Soprano::LiteralValue literal = QString::fromStdString(str);
            Soprano::Node tmp = Soprano::Node::createLiteralNode(literal);
            return tmp.toN3().toStdString();
        }
    };


    /**
        Alternative to n3_string: The plain_string trait defines methods to create a plain string
        from a given value, without any type-extensions, i.e.
            plain_string::from(3.5);
        will return the string:
            5
        whereas
            n3_string::from(3.5);
        would return something like:
            "3.5000e+00"^^<http://www.w3.org/2001/XMLSchema#double>

        The default implementation uses std::to_string.
    */
    template <typename T, typename Enable = void>
    struct plain_string
    {
        static std::string from(const T& value)
        {
            return std::to_string(value);
        }
    };

    /**
        Specialization for Entity-pointer
    */
    template <typename T>
    struct plain_string<
                std::shared_ptr<T>,
                typename std::enable_if<std::is_base_of<sempr::entity::Entity, T>::value>::type
            >
    {
        static std::string from(std::shared_ptr<T> value)
        {
            if (!value) return "";
            return value->id();
        }
    };

    /**
        Specialization for strings
    */
    template <>
    struct plain_string<std::string, void>
    {
        static std::string from(const std::string& value)
        {
            return value;
        }
    };

} /* traits */
} /* rdf */



namespace entity {

/**
    The RegisteredPropertyBase class is the interface for handling any variable that should be
    represented as the "object" part of an rdf triple. It also stores the predicate to use and
    provides a way to also add a custom, optional subject. If the latter is not given the
    SemanticEntityIterator_impl will use the id of the SemanticEntity as the subject.

    The virtual std::string object() const method needs to be overridden to return the string that
    will be used in the triple.

    The virtual bool isValid() const method can return false if the current value of the property
    is invalid and the resulting triple should not be published.
*/
class RegisteredPropertyBase {
protected:
    bool hasSubject_;     // use the given subject string or the id of the owner?
                             // if false, "<sempr:" + this->id() + ">" will be used.
    std::string subject_;    // custom subject string
    std::string predicate_;  // predicate (e.g. "<http://some.ontology/age>")

    RegisteredPropertyBase(const std::string& predicate);
    RegisteredPropertyBase(const std::string& subject, const std::string& predicate);

public:
    virtual ~RegisteredPropertyBase(){}

    bool hasSubject() const;
    std::string subject() const;
    std::string predicate() const;
    virtual std::string object() const = 0;
    virtual bool isValid() const = 0;
};

/**
    The RegisteredProperty<T> simply adds a method to return the wrapped value as a string. It is
    only used inside the SemanticEntity to keep track of all the registered member variables.

    For usage see SemanticEntity::registerProperty

    As a second template argument a type trait called "ToString" can be provided, which will be
    used to actually convert the registered value to a string. It must define a static method
        std::string ToString<T>::from(const T& value)
    The default traits used is sempr::rdf::traits::n3_string which will add type information to the
    values stored in the rdf triples. An alternative is sempr::rdf::traits::plain_string which omits
    any additional information and uses std::to_string to convert plain literals, returns
    entity->id() for any Entity::Ptr, and returns strings unchanged.
*/
template <class T, template <class ...> class ToString = sempr::rdf::traits::n3_string>
class RegisteredProperty : public RegisteredPropertyBase {
    friend class SemanticEntity;

    T& value_;  // a reference to the wrapped variable

    RegisteredProperty(const std::string& predicate, T& value)
        : RegisteredPropertyBase(predicate), value_(value)
    {
    }

    RegisteredProperty(const std::string& subject, const std::string& predicate, T& value)
        : RegisteredPropertyBase(subject, predicate), value_(value)
    {
    }

    /**
        Returns a string representation of the value which is used in the corresponding triple.
    */
    std::string object() const override
    {
        return ToString<T>::from(value_);
    }

    /**
        The default assumption is that the value is always valid, since we get the variable by
        reference. The specialization for smart-pointers checks if the pointer is not null.
    */
    bool isValid() const override
    {
        return isValid_impl<T>();
    }

    template <class U>
    bool isValid_impl(typename std::enable_if<core::is_shared_ptr<U>::value, void*>::type = 0) const
    {
        return value_.get() != nullptr;
    }

    template <class U>
    bool isValid_impl(typename std::enable_if<not core::is_shared_ptr<U>::value, void*>::type = 0) const
    {
        return true;
    }


};



/**
    While the RDFPropertyMap allows us to add almost any type of property dynamically to an
    entity, it does so at the cost of type safety and string-identifiers for the properties.

    This is a proposal for a different approach: The SemanticEntity allows us to register
    any member variable, for which a wrapper-object (RegisteredProperty) will be stored. Via the
    type traits method
        static std::string sempr::rdf::traits::value_string<T>::from(const T&);
    it is possible to extend the supported types and define how they will appear in the rdf triples.

    This way, the values are stored directly in the objects db table, clearly visible and without
    any overhead, but are also available as rdf triples. Use it like this (simplified):

    class MyEntity : public SemanticEntity {
        int myInt_;
        float myFloat_;

    public:
        MyEntity() {
            registerPropetry("<http://some.ontology/somePredicate>", myInt_);
            registerProperty("<http://some.ontology/someSubject>", "<http://some.ontology/somePredicate>", myFloat_);
        }
    };

    The SemanticEntity will construct the triples from the values on demand when it is iterated.
*/
#pragma db object
class SemanticEntity : public RDFEntity {
    SEMPR_ENTITY

    friend class odb::access;

    #pragma db transient
    std::vector<RegisteredPropertyBase*> properties_;

protected:
    SemanticEntity(const core::IDGenBase*);
    SemanticEntity();

    /**
        Registers the property at this semantic entity. This will provide the RDF-Triple:
            <sempr:SemanticEntity_#> <predicate> sempr::rdf::traits::value_string<T>::from(property)

        The default value for the subject is the ID of this SemanticEntity.
        Assumes the given property it a member of this SemanticEntity.
    */
    template <class T>
    void registerProperty(const std::string& predicate, T& property)
    {
        RegisteredPropertyBase* rprop = new RegisteredProperty<T>(predicate, property);
        properties_.push_back(rprop);
    }

    template <class T>
    void registerPropertyPlain(const std::string& predicate, T& property)
    {
        RegisteredPropertyBase* rprop =
            new RegisteredProperty<T, sempr::rdf::traits::plain_string>(predicate, property);
        properties_.push_back(rprop);
    }

    /**
        Registers the property at this semantic entity. This allows setting both subject and
        predicate of the resulting triple.

        Assumes the given property is a member of this SemanticEntity.
    */
    template <class T>
    void registerProperty(const std::string& subject, const std::string& predicate,
                          T& property)
    {
        RegisteredPropertyBase* rprop = new RegisteredProperty<T>(subject, predicate, property);
        properties_.push_back(rprop);
    }

    template <class T>
    void registerPropertyPlain(const std::string& subject, const std::string& predicate, T& property)
    {
        RegisteredPropertyBase* rprop =
            new RegisteredProperty<T, sempr::rdf::traits::plain_string>(subject, predicate, property);
        properties_.push_back(rprop);
    }

public:
    ~SemanticEntity();

    TripleIterator begin() const override;
    TripleIterator end() const override;
};


class SemanticEntityIterator_impl : public TripleIterator_impl {
    friend class SemanticEntity;

    typedef std::vector<RegisteredPropertyBase*>::const_iterator ConstIterator;
    ConstIterator vit_, end_;
    const SemanticEntity* entity_;

    SemanticEntityIterator_impl(ConstIterator it, ConstIterator end, const SemanticEntity* entity);

    const Triple operator *() const override;
    void operator ++() override;
    bool operator == (const TripleIterator_impl& other) const override;

    /**
        There may be invalid values inside the list, namely nullptrs, which would lead to invalid
        triples. To avoid these the RegisteredProperty provides an isValid method that can be used
        to skip those entries.

        If the current property that this iterator is pointing at is invalid, this method advances
        up to the next valid property or end. If the current property is valid this method does
        nothing.

        Since the iterator now needs to know the end itself it is passed in the ctor.
    */
    void makeValid();
};

} // ns entity
} // ns sempr

#endif /* end of include guard: SEMPR_ENTITY_SEMANTICENTITY_HPP */
