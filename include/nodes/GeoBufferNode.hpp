#ifndef SEMPR_GEOBUFFERNODE_HPP_
#define SEMPR_GEOBUFFERNODE_HPP_

#include <rete-core/Builtin.hpp>
#include <rete-core/Accessors.hpp>
#include <rete-core/builtins/NumberToNumberConversion.hpp>

#include "../component/GeosGeometry.hpp"

namespace sempr {

/**
    Buffers a geometry by a given offset
*/
class GeoBufferNode : public rete::Builtin {
public:
    GeoBufferNode(
            rete::PersistentInterpretation<GeosGeometryInterface::Ptr> geo,
            rete::builtin::NumberToNumberConversion<double> offset
    );

    rete::WME::Ptr process(rete::Token::Ptr) override;
    bool operator == (const rete::BetaNode& other) const override;

private:
    rete::PersistentInterpretation<GeosGeometryInterface::Ptr> geo_;
    rete::builtin::NumberToNumberConversion<double> offset_;
};

}

#endif /* include guard: SEMPR_GEOBUFFERNODE_HPP_ */
