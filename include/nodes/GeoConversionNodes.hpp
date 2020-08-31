#ifndef SEMPR_GEOCONVERSIONNODES_HPP_
#define SEMPR_GEOCONVERSIONNODES_HPP_

#include <rete-core/Builtin.hpp>
#include <rete-core/Accessors.hpp>

#include "../component/GeosGeometry.hpp"

namespace sempr {

/**
    Takes a geometry and a zone (int), and returns a new geometry transformed
    from WGS84 lon/lat to UTM<zone> x/y.
*/
class UTMFromWGSNode : public rete::Builtin {
public:
    UTMFromWGSNode(
            rete::PersistentInterpretation<GeosGeometryInterface::Ptr> geo,
            rete::PersistentInterpretation<int> zone
    );

    rete::WME::Ptr process(rete::Token::Ptr) override;

    bool operator == (const rete::BetaNode& other) const override;

private:
    rete::PersistentInterpretation<GeosGeometryInterface::Ptr> geo_;
    rete::PersistentInterpretation<int> zone_;
};


}


#endif /* include guard: SEMPR_GEOCONVERSIONNODES_HPP_ */
