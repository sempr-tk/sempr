#ifndef SEMPR_TUPLECOMPONENTACCESSOR_HPP_
#define SEMPR_TUPLECOMPONENTACCESSOR_HPP_

#include <rete-core/TupleWMEAccessor.hpp>
#include "Component.hpp"

namespace rete {

/**
    Specialization of TupleWMEAccessors:
    If the referenced type is a shared_ptr to a class which is derived from
    sempr::Component, the accessor also provides an interpretation as a
    sempr::Component::Ptr.
*/
template <size_t I, typename TWME, typename T>
class TupleWMEAccessor<
        I, TWME, T,
        typename
            std::enable_if<
                sempr::is_shared_ptr<T>::value and
                std::is_base_of<
                    sempr::Component,
                    typename sempr::remove_shared_ptr<T>::type
                >::value,
                void
            >::type>
    :
        public rete::Accessor<TWME, T, sempr::Component::Ptr> {
public:
    bool equals(const rete::AccessorBase& other) const override
    {
        auto o = dynamic_cast<const TupleWMEAccessor*>(&other);
        return o != nullptr;
    }

    void getValue(std::shared_ptr<TWME> wme, T& value) const override
    {
        value = std::get<I>(wme->value_);
    }

    void getValue(std::shared_ptr<TWME> wme, sempr::Component::Ptr& value) const override
    {
        value = std::static_pointer_cast<sempr::Component>(std::get<I>(wme->value_));
    }

    TupleWMEAccessor* clone() const override
    {
        auto acc = new TupleWMEAccessor();
        acc->index_ = this->index_;
        return acc;
    }

};

}

#endif /* include guard: SEMPR_TUPLECOMPONENTACCESSOR_HPP_ */
