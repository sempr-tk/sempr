#ifndef SEMPR_TEXTCOMPONENTTEXTNODE_HPP_
#define SEMPR_TEXTCOMPONENTTEXTNODE_HPP_

#include <rete-core/Builtin.hpp>
#include <rete-core/Accessors.hpp>

#include "../component/TextComponent.hpp"

namespace sempr {

/**
    This node extracts the text value from a TextComponent.
    Propagates a TupleWME<std::string> with the value.
*/
class TextComponentTextNode : public rete::Builtin {
    std::unique_ptr<rete::SpecificTypeAccessor<TextComponent::Ptr>> component_;
public:
    TextComponentTextNode(
        std::unique_ptr<rete::SpecificTypeAccessor<TextComponent::Ptr>> c
    );

    rete::WME::Ptr process(rete::Token::Ptr) override;

    bool operator == (const rete::BetaNode& other) const override;

};


}

#endif /* include guard: SEMPR_TEXTCOMPONENTTEXTNODE_HPP_ */
