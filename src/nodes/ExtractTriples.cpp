#include "nodes/ExtractTriples.hpp"

namespace sempr {

ExtractTriples::ExtractTriples(
        std::unique_ptr<ComponentAccessor<TripleContainer>> container)
    : accessor_(std::move(container))
{
}

void ExtractTriples::execute(
        rete::Token::Ptr token,
        rete::PropagationFlag flag,
        std::vector<rete::WME::Ptr>& inferred)
{
    if (flag == rete::PropagationFlag::ASSERT || flag == rete::PropagationFlag::UPDATE)
    {
        TripleContainer::Ptr container;
        accessor_->getValue(token, container);

        for (auto triple : *container)
        {
            auto wme = std::make_shared<rete::Triple>(
                triple.getField(Triple::Field::SUBJECT),
                triple.getField(Triple::Field::PREDICATE),
                triple.getField(Triple::Field::OBJECT)
            );

            inferred.push_back(wme);
        }
    }
}


std::string ExtractTriples::toString() const
{
    return "ExtractTriples(" + accessor_->toString() + ")";
}


}
