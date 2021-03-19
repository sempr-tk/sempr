#include "nodes/DateMonthBuilder.hpp"
#include "nodes/DateMonthNode.hpp"

#include <rete-core/TupleWMEAccessor.hpp>
#include <rete-core/TupleWME.hpp>
#include <rete-core/builtins/NumberToNumberConversion.hpp>

namespace sempr {


DateMonthBuilder::DateMonthBuilder()
    : rete::NodeBuilder("date:month", rete::NodeBuilder::BuilderType::BUILTIN)
{
}


rete::Builtin::Ptr DateMonthBuilder::buildBuiltin(rete::ArgumentList& args) const
{
    // exactly 2 args: month, date
    if (args.size() != 2) throw rete::NodeBuilderException("Invalid number of arguments (!=2)");
    if (args[0].isConst() || args[0].getAccessor()) throw rete::NodeBuilderException("First argument must be unbound, as it is reserved for the result.");

    rete::PersistentInterpretation<std::string> date;
    if (args[1].isConst())
    {
        if (args[1].getAST().isString())
        {
            rete::ConstantAccessor<std::string> acc(args[1].getAST().toString());
            acc.index() = 0;
            date = acc.getInterpretation<std::string>()->makePersistent();
        }
        else
        {
            throw rete::NodeBuilderException("Date parameter is not a string");
        }
    }
    else /* isVariable */
    {
        auto acc = args[1].getAccessor();
        if (acc)
        {
            auto strInterp = acc->getInterpretation<std::string>();
            if (strInterp)
                date = strInterp->makePersistent();
            else
                throw rete::NodeBuilderException(
                        "Variable " + args[1].getVariableName() +
                        " is not a string");
        }
        else
        {
            throw rete::NodeBuilderException(
                    "Variable " + args[1].getVariableName() +
                    " is unbound. Must be a string.");
        }
    }
    // create the node
    auto node = std::make_shared<DateMonthNode>(std::move(date));

    // bind the result variable to an accessor matching the return of DateMonthNode
    auto resultAccessor = std::make_shared<rete::TupleWME<int>::Accessor<0>>();

    args[0].bind(resultAccessor);

    return node;
}


}
