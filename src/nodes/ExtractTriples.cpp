#include "nodes/ExtractTriples.hpp"

namespace sempr {

ExtractTriples::ExtractTriples(
        rete::PersistentInterpretation<TripleContainer::Ptr> container)
    : container_(std::move(container))
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
        container_.interpretation->getValue(token, container);

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
    return "ExtractTriples(" + container_.accessor->toString() + ")";
}


}
