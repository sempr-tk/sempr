#include <rete-core/Accessors.hpp>
#include <rete-core/TupleWMEAccessor.hpp>
#include <rete-core/TupleWME.hpp>

#include "nodes/AffineTransformNodes.hpp"
#include "nodes/AffineTransformBuilders.hpp"

namespace sempr {


// ----------------------------------------------------------------------------
// tf:create
// ----------------------------------------------------------------------------
AffineTransformCreateBuilder::AffineTransformCreateBuilder()
    : rete::NodeBuilder("tf:create", rete::NodeBuilder::BuilderType::BUILTIN)
{
}

rete::Builtin::Ptr AffineTransformCreateBuilder::buildBuiltin(rete::ArgumentList& args) const
{
    // there must be exactly 8 arguments: 1 result, 3 translation, 4 rotation (xyzw)
    // TODO: allow 3 params (let q = (0 0 0 1))?
    if (args.size() != 8) throw rete::NodeBuilderException("Invalid number of arguments (!= 8)");
    if (args[0].isConst() || args[0].getAccessor()) throw rete::NodeBuilderException("First argument must be unbound variable to store the result in");

    rete::PersistentInterpretation<float> params[7];
    for (int i = 1; i < 8; i++)
    {
        if (args[i].isConst())
        {
            // create const accessors for constants in rule defs
            auto acc = new rete::ConstantAccessor<float>(args[i].getAST().toFloat());
            // Allow the accessor to be applied to a token.
            acc->index() = 0;

            // save it in a unified format
            params[i-1] = acc->getInterpretation<float>()->makePersistent();
        }
        else
        {
            if (!args[i].getAccessor())
            {
                throw rete::NodeBuilderException("Argument " + std::to_string(i) + " (" + args[i].getVariableName() + ") "
                                                 "is unbound!");
            }

            auto interpretation = args[i].getAccessor()->getInterpretation<float>();
            if (!interpretation)
            {
                throw rete::NodeBuilderException("Argument " + std::to_string(i) + " (" + args[i].getVariableName() + ") "
                                                 "is incompatible to rete::NumberAccessor");
            }

            // all good. save a persistent copy
            params[i-1] = interpretation->makePersistent();
        }
    }

    // create the node
    auto node = std::make_shared<AffineTransformCreate>(
            std::move(params[0]), std::move(params[1]), std::move(params[2]), // x y z
            std::move(params[3]), std::move(params[4]), std::move(params[5]), // qx qy qz
            std::move(params[6])                                              // qw
    );

    // bind the result variable to the output of the node
    // make sure that the types match.
    // A TupleWMEAccessor for a TupleWME<AffineTransform::Ptr> because we *know*
    // that the node returns a  TupleWME<AffineTransform::Ptr>.
    auto resultAccessor = std::make_shared<rete::TupleWMEAccessor<0, rete::TupleWME<AffineTransform::Ptr>>>();

    // tf:create(?tf 1 2 3 0 0 0 1)
    //   ?tf = resultAccessor
    args[0].bind(resultAccessor);

    return node;
}

// ----------------------------------------------------------------------------
// tf:get
// ----------------------------------------------------------------------------
AffineTransformGetBuilder::AffineTransformGetBuilder()
    : rete::NodeBuilder("tf:get", rete::NodeBuilder::BuilderType::BUILTIN)
{
}

rete::Builtin::Ptr AffineTransformGetBuilder::buildBuiltin(rete::ArgumentList& args) const
{
    // there must be at least 2, at max 8 arguments.
    // tf:get(?tf ?x [?y ?z ?qx ?qy ?qz ?qw])
    if (args.size() < 2 || args.size() > 8)
        throw rete::NodeBuilderException("Invalid number of arguments. Must be 2 <= n <= 8");
    // the first argument must be bound to an AffineTransform
    if (args[0].isConst()) throw rete::NodeBuilderException("First argument must be AffineTransform, but is some constant.");
    if (!args[0].getAccessor()) throw rete::NodeBuilderException("First argument must be AffineTransform, but is unbound.");

    auto interpretation = args[0].getAccessor()->getInterpretation<AffineTransform::Ptr>();
    if (!interpretation)
        throw rete::NodeBuilderException("First argument must be bound to an AffineTransform, nothing else.");

    // all other arguments must be unbound variables to store the results in
    for (size_t i = 1; i < args.size(); i++)
    {
        if (args[i].isConst() || args[i].getAccessor())
            throw rete::NodeBuilderException("Argument " + std::to_string(i) + " must be an unbound variable.");
    }

    // create and bind accessors.
    typedef rete::TupleWME<double, double, double,
                           double, double, double, double> ResultWME;
    args[1].bind(std::make_shared<rete::TupleWMEAccessor<0, ResultWME>>()); // ?x
    if (args.size() > 2) args[2].bind(std::make_shared<rete::TupleWMEAccessor<1, ResultWME>>()); // ?y
    if (args.size() > 3) args[3].bind(std::make_shared<rete::TupleWMEAccessor<2, ResultWME>>()); // ?z
    if (args.size() > 4) args[4].bind(std::make_shared<rete::TupleWMEAccessor<3, ResultWME>>()); // ?qx
    if (args.size() > 5) args[5].bind(std::make_shared<rete::TupleWMEAccessor<4, ResultWME>>()); // ?qy
    if (args.size() > 6) args[6].bind(std::make_shared<rete::TupleWMEAccessor<5, ResultWME>>()); // ?qz
    if (args.size() > 7) args[7].bind(std::make_shared<rete::TupleWMEAccessor<6, ResultWME>>()); // ?qw

    // create the node
    auto node = std::make_shared<AffineTransformGet>(interpretation->makePersistent());

    return node;
}

// ----------------------------------------------------------------------------
// tf:mul
// ----------------------------------------------------------------------------
AffineTransformMulBuilder::AffineTransformMulBuilder()
    : rete::NodeBuilder("tf:mul", rete::NodeBuilder::BuilderType::BUILTIN)
{
}

rete::Builtin::Ptr AffineTransformMulBuilder::buildBuiltin(rete::ArgumentList& args) const
{
    // exactly 3 arguments: one unbound result variable and two bound to AffineTransforms.
    if (args.size() != 3)
        throw rete::NodeBuilderException("Invalid number of arguments (!= 3)");

    if (args[0].isConst() || args[0].getAccessor())
        throw rete::NodeBuilderException("First argument must be unbound, for the result.");

    // second: variable bound to AffineTransform.
    if (args[1].isConst() || !args[1].getAccessor() ||
        !args[1].getAccessor()->getInterpretation<AffineTransform::Ptr>())
        throw rete::NodeBuilderException("Second argument must be bound to an AffineTransform.");

    // third: variable bound to AffineTransform
    if (args[1].isConst() || !args[1].getAccessor() ||
        !args[1].getAccessor()->getInterpretation<AffineTransform::Ptr>())
        throw rete::NodeBuilderException("Second argument must be bound to an AffineTransform.");

    // clone the accessors
    rete::PersistentInterpretation<AffineTransform::Ptr>
        left(args[1].getAccessor()->getInterpretation<AffineTransform::Ptr>()->makePersistent()),
        right(args[2].getAccessor()->getInterpretation<AffineTransform::Ptr>()->makePersistent());

    // create the node
    auto node = std::make_shared<AffineTransformMul>(std::move(left), std::move(right));

    // bind result
    auto resultAccessor = std::make_shared<rete::TupleWMEAccessor<0, rete::TupleWME<AffineTransform::Ptr>>>();
    args[0].bind(resultAccessor);

    return node;
}

// ----------------------------------------------------------------------------
// tf:inv
// ----------------------------------------------------------------------------
AffineTransformInvBuilder::AffineTransformInvBuilder()
    : rete::NodeBuilder("tf:inv", rete::NodeBuilder::BuilderType::BUILTIN)
{
}

rete::Builtin::Ptr AffineTransformInvBuilder::buildBuiltin(rete::ArgumentList& args) const
{
    // exactly 2 arguments needed, unbound result and bound AffineTransform
    if (args.size() != 2) throw rete::NodeBuilderException("Invalid number of arguments (!=2)");
    if (args[0].isConst() || args[0].getAccessor()) throw rete::NodeBuilderException("First argument must be an unbound variable.");
    if (args[1].isConst() || !args[1].getAccessor() ||
        !args[1].getAccessor()->getInterpretation<AffineTransform::Ptr>())
        throw rete::NodeBuilderException("Second argument must be bound to an AffineTransform.");

    // clone accessor
    rete::PersistentInterpretation<AffineTransform::Ptr>
        input(args[1].getAccessor()->getInterpretation<AffineTransform::Ptr>()->makePersistent());

    // construct node
    auto node = std::make_shared<AffineTransformInv>(std::move(input));

    // bind result
    auto resultAccessor = std::make_shared<rete::TupleWMEAccessor<0, rete::TupleWME<AffineTransform::Ptr>>>();
    args[0].bind(resultAccessor);

    return node;
}

}
