#ifndef SEMPR_AFFINETRANSFORM_GET_NODE_
#define SEMPR_AFFINETRANSFORM_GET_NODE_

#include <rete-core/Builtin.hpp>
#include <rete-core/Util.hpp>
#include <rete-core/Accessors.hpp>

#include "component/AffineTransform.hpp"

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

    /**
        Equality-check for possible reusability of nodes
    */
    bool operator == (const rete::BetaNode& other) const override;
};

/**
    This node extracts the parameters of the AffineTransform component into
    a Tuple<double, double, ...>. x,y,z,qx,qy,qz,qw.

    Why is this necessary? You might think that implementing a special accessor
    for the AffineTransform-component would suffice. But that is not possible,
    since AffineTransform is *not* a WME! It is stored in a WME, e.g. ECWME or
    TupleWME<AffineTransform::Ptr>. Hence we need this node to take an accessor
    that gives us an AffineTransform from a WME/token, and store the values
    directly in a TupleWME so that our node builder can hand out accessors to
    the values in the TupleWME. (Think of levels of indirection. This nodes
    makes the transition from "Value in Value in WME" to "Value in WME".)
*/
class AffineTransformGet : public rete::Builtin {
    std::unique_ptr<rete::SpecificTypeAccessor<AffineTransform::Ptr>> tf_;
public:
    AffineTransformGet(std::unique_ptr<rete::SpecificTypeAccessor<AffineTransform::Ptr>> tf);

    /**
        Extracts the parameters. Returns a TupleWME<double, double, ..., double>
        (x, y, z, qx, qy, qz, qw)
    */
    rete::WME::Ptr process(rete::Token::Ptr) override;

    /**
        Equality-check for possible reusability of nodes
    */
    bool operator == (const rete::BetaNode& other) const override;
};


/**
    Multiplication of AffineTransforms
*/
class AffineTransformMul : public rete::Builtin {
    std::unique_ptr<rete::SpecificTypeAccessor<AffineTransform::Ptr>> left_, right_;
public:
    // for multiplication of two AffineTransforms.
    AffineTransformMul(
        std::unique_ptr<rete::SpecificTypeAccessor<AffineTransform::Ptr>> left,
        std::unique_ptr<rete::SpecificTypeAccessor<AffineTransform::Ptr>> right
    );

    rete::WME::Ptr process(rete::Token::Ptr) override;

    /**
        Equality-check for possible reusability of nodes
    */
    bool operator == (const rete::BetaNode& other) const override;

};

/**
    Compute inverse
*/
class AffineTransformInv : public rete::Builtin {
    std::unique_ptr<rete::SpecificTypeAccessor<AffineTransform::Ptr>> tf_;
public:
    AffineTransformInv(
        std::unique_ptr<rete::SpecificTypeAccessor<AffineTransform::Ptr>> tf
    );

    rete::WME::Ptr process(rete::Token::Ptr) override;

    /**
        Equality-check for possible reusability of nodes
    */
    bool operator == (const rete::BetaNode& other) const override;
};

}

#endif /* include guard: SEMPR_AFFINETRANSFORM_GET_NODE_ */
