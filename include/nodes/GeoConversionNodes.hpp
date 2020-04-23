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
    typedef rete::SpecificTypeAccessor<std::shared_ptr<GeosGeometryInterface>> accessor_t;
    UTMFromWGSNode(
            std::unique_ptr<accessor_t> geo,
            std::unique_ptr<rete::NumberAccessor> zone
    );

    rete::WME::Ptr process(rete::Token::Ptr) override;

    bool operator == (const rete::BetaNode& other) const override;

private:
    std::unique_ptr<accessor_t> geo_;
    std::unique_ptr<rete::NumberAccessor> zone_;

};


}


#endif /* include guard: SEMPR_GEOCONVERSIONNODES_HPP_ */
