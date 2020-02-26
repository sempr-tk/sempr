#ifndef SEMPR_AFFINETRANSFORMBUILDERS_HPP_
#define SEMPR_AFFINETRANSFORMBUILDERS_HPP_

#include <rete-reasoner/NodeBuilder.hpp>
#include <rete-reasoner/Exceptions.hpp>

#include "nodes/AffineTransformNodes.hpp"
#include "Utility.hpp"

namespace sempr {

/**
    Builds the tf:create(tf x y z qx qy qz qw) nodes, which create new tfs
    from the given parameters.
*/
class AffineTransformCreateBuilder : public rete::NodeBuilder {
public:
    AffineTransformCreateBuilder();
    rete::Builtin::Ptr buildBuiltin(rete::ArgumentList& args) const override;
};

/**
    Builds the tf:get(tf x y z qx qy qz qw) nodes, which extract the parameters
    from a given tf.
*/
class AffineTransformGetBuilder : public rete::NodeBuilder {
public:
    AffineTransformGetBuilder();
    rete::Builtin::Ptr buildBuiltin(rete::ArgumentList& args) const override;
};

/**
    Builds the tf:mul(result left right) nodes. result = left * right on
    AffineTransforms.
*/
class AffineTransformMulBuilder : public rete::NodeBuilder {
public:
    AffineTransformMulBuilder();
    rete::Builtin::Ptr buildBuiltin(rete::ArgumentList& args) const override;
};

/**
    Builds the tf:inv(result input) nodes. result = input^(-1) on
    AffineTransforms.
*/
class AffineTransformInvBuilder : public rete::NodeBuilder {
public:
    AffineTransformInvBuilder();
    rete::Builtin::Ptr buildBuiltin(rete::ArgumentList& args) const override;
};



}

#endif /* include guard: SEMPR_AFFINETRANSFORMBUILDERS_HPP_ */
