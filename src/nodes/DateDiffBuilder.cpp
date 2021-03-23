#include "nodes/DateDiffBuilder.hpp"
#include "nodes/DateDiffNode.hpp"

#include <rete-core/TupleWMEAccessor.hpp>
#include <rete-core/TupleWME.hpp>
#include <rete-core/builtins/NumberToNumberConversion.hpp>

namespace sempr {

DateDiffBuilder::DateDiffBuilder()
    : rete::NodeBuilder("date:diff", rete::NodeBuilder::BuilderType::BUILTIN)
{
}

rete::Builtin::Ptr DateDiffBuilder::buildBuiltin(rete::ArgumentList& args) const
{
    // exactly 3 args: diff, date1, date2
    if (args.size() != 3) throw rete::NodeBuilderException("Invalid number of arguments (!=3)");
    if (args[0].isConst() || args[0].getAccessor()) throw rete::NodeBuilderException("First argument must be unbound, as it is reserved for the result.");

    rete::PersistentInterpretation<std::string> date1;
    rete::PersistentInterpretation<std::string> date2;

    if (args[1].isConst())
    {
        if (args[1].getAST().isString())
        {
            rete::ConstantAccessor<std::string> acc(args[1].getAST().toString());
            acc.index() = 0;
            date1 = acc.getInterpretation<std::string>()->makePersistent();
        }
        else
        {
            throw rete::NodeBuilderException("Second parameter is not a string");
        }
    }
    else /* isVariable */
    {
        auto acc = args[1].getAccessor();
        if (acc)
        {
            auto strInterp = acc->getInterpretation<std::string>();
            if (strInterp)
                date1 = strInterp->makePersistent();
            else
                throw rete::NodeBuilderException(
                        "Variable " + args[1].getVariableName() + " is not a string");
        }
        else
        {
            throw rete::NodeBuilderException(
                    "Variable " + args[1].getVariableName() + " is unbound. Must be a string.");
        }
    }
    
    if (args[2].isConst())
    {
        if (args[2].getAST().isString())
        {
            rete::ConstantAccessor<std::string> acc(args[2].getAST().toString());
            acc.index() = 0;
            date2 = acc.getInterpretation<std::string>()->makePersistent();
        }
        else
        {
            throw rete::NodeBuilderException("Third parameter is not a string");
        }
    }
    else
    {
        auto acc2 = args[2].getAccessor();
        if (acc2)
        {
            auto strInterp = acc2->getInterpretation<std::string>();
            if (strInterp)
                date2 = strInterp->makePersistent();
            else
                throw rete::NodeBuilderException(
                        "Variable " + args[2].getVariableName() + " is not a string");
        }
        else
        {
            throw rete::NodeBuilderException(
                    "Variable " + args[2].getVariableName() + " is unbound. Must be a string.");
        }
    }   
    
    // create the node
    auto node = std::make_shared<DateDiffNode>(std::move(date1), std::move(date2));

    // bind the result variable to an accessor matching the return of DateDiffNode
    auto resultAccessor = std::make_shared<rete::TupleWME<int>::Accessor<0>>();
   
    args[0].bind(resultAccessor);
    
    return node;
}


}
