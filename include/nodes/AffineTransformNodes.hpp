#ifndef SEMPR_AFFINETRANSFORM_GET_NODE_
#define SEMPR_AFFINETRANSFORM_GET_NODE_

#include <rete-core/Builtin.hpp>
#include <rete-core/Util.hpp>
#include <rete-core/Accessors.hpp>


namespace sempr {

/**
    This node creates a new affine transformation from the given parameters.
*/
class AffineTransformCreate : public rete::Builtin {
    std::unique_ptr<rete::NumberAccessor> x_, y_, z_, qx_, qy_, qz_, qw_;
public:
    // all values must be given when creating a new affine transformation.
    AffineTransformCreate(
        std::unique_ptr<rete::NumberAccessor> x,
        std::unique_ptr<rete::NumberAccessor> y,
        std::unique_ptr<rete::NumberAccessor> z,
        std::unique_ptr<rete::NumberAccessor> qx,
        std::unique_ptr<rete::NumberAccessor> qy,
        std::unique_ptr<rete::NumberAccessor> qz,
        std::unique_ptr<rete::NumberAccessor> qw
    );


    /**
        Actual implementation. Returns a TupleWME<AffineTransform> containing
        the newly created AffineTransform.
    */
    rete::WME::Ptr process(rete::Token::Ptr) override;


};

}

#endif /* include guard: SEMPR_AFFINETRANSFORM_GET_NODE_ */
