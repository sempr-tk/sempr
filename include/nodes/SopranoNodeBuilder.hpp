#ifndef SEMPR_SOPRANOMODULEBUILDER_HPP_
#define SEMPR_SOPRANOMODULEBUILDER_HPP_

#include <rete-reasoner/NodeBuilder.hpp>
#include <rete-reasoner/Exceptions.hpp>

#include "SopranoModule.hpp"
#include "../Utility.hpp"

namespace sempr {

/**
    Constructs the SopranoNodes.
    Takes a SopranoModule::Ptr as input to which all SopranoNodes will
    be linked.
*/
class SopranoNodeBuilder : public rete::NodeBuilder {
    SopranoModule::Ptr module_;

    void argToAccessor(rete::Argument& arg, rete::PersistentInterpretation<std::string>& acc) const;
public:
    SopranoNodeBuilder(SopranoModule::Ptr);
    rete::Production::Ptr buildEffect(rete::ArgumentList& args) const override;
};


}
#endif /* include guard: SEMPR_SOPRANOMODULEBUILDER_HPP_ */

