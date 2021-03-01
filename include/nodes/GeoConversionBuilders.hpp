#ifndef SEMPR_GEOCONVERSIONBUILDERS_HPP_
#define SEMPR_GEOCONVERSIONBUILDERS_HPP_

#include <rete-reasoner/NodeBuilder.hpp>

namespace sempr {

/**
    geo:UTMFromWGS(?newGeometry ?oldGeometry ?targetUTMZone)
*/
class UTMFromWGSBuilder : public rete::NodeBuilder {
public:
    UTMFromWGSBuilder();
    rete::Builtin::Ptr buildBuiltin(rete::ArgumentList& args) const override;
};


/**
    geo:WGSFromUTM(?newGeometry ?oldGeometry ?sourceUTMZone)
*/
class WGSFromUTMBuilder : public rete::NodeBuilder {
public:
    WGSFromUTMBuilder();
    rete::Builtin::Ptr buildBuiltin(rete::ArgumentList& args) const override;
};

}

#endif /* include guard: SEMPR_GEOCONVERSIONBUILDERS_HPP_ */
