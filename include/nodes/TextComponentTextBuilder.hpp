#ifndef SEMPR_TEXTCOMPONENTTEXTBUILDER_HPP_
#define SEMPR_TEXTCOMPONENTTEXTBUILDER_HPP_

#include <rete-reasoner/NodeBuilder.hpp>

namespace sempr {

class TextComponentTextBuilder : public rete::NodeBuilder {
public:
    TextComponentTextBuilder();
    rete::Builtin::Ptr buildBuiltin(rete::ArgumentList& args) const override;
};


}

#endif /* include guard: SEMPR_TEXTCOMPONENTTEXTBUILDER_HPP_ */
