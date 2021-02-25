#ifndef SEMPR_TUPLEENTITYACCESSOR_HPP_
#define SEMPR_TUPLEENTITYACCESSOR_HPP_

#include <rete-core/Accessors.hpp>
#include <rete-rdf/TriplePart.hpp>

#include <rete-core/TupleWME.hpp>
#include <rete-core/TupleWMEAccessor.hpp>

#include "Entity.hpp"
#include "RDF.hpp"

// specialization of rete::TupleWMEAccessor!!
namespace rete {

/**
    This accessor is supposed to behave the same as a sempr::EntityAccessor,
    with the difference being that the sempr::EntityAccessor only works for
    ECWMEs. This here is a specialization of the TupleWMEAccessor, such that
    a Entity::Ptr can also be accessed from other tuples, like a simple
    rete::TupleWME<sempr::Entity::Ptr> as returned by e.g. the createEntity
    builtin.
*/
template <size_t I, typename TWME>
class TupleWMEAccessor<I, TWME, sempr::Entity::Ptr>
    : public Accessor<TWME, sempr::Entity::Ptr, rete::TriplePart, std::string> {

public:
    bool equals(const rete::AccessorBase& other) const override
    {
        auto o = dynamic_cast<const TupleWMEAccessor*>(&other);
        return o != nullptr;
    }

    void getValue(std::shared_ptr<TWME> wme, sempr::Entity::Ptr& value) const override
    {
        value = std::get<I>(wme->value_);
    }

    void getValue(std::shared_ptr<TWME> wme, rete::TriplePart& value) const override
    {
        auto& e = std::get<I>(wme->value_);
        if (e->idIsURI())
            value.value = e->id();
        else
            value.value = "<" + sempr::baseURI() + std::get<I>(wme->value_)->id() + ">";
    }

    void getValue(std::shared_ptr<TWME> wme, std::string& value) const override
    {
        value = std::get<I>(wme->value_)->id();
    }

    TupleWMEAccessor* clone() const override
    {
        auto acc = new TupleWMEAccessor();
        acc->index_ = this->index_;
        return acc;
    }

    std::string toString() const override
    {
        return "Entity[" + std::to_string(this->index_) + "][" + std::to_string(I) + "]";
    }
};

}

#endif /* include guard: SEMPR_TUPLEENTITYACCESSOR_HPP_ */
