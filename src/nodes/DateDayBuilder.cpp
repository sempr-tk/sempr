#include "nodes/DateDayBuilder.hpp"
#include "nodes/DateDayNode.hpp"

#include <rete-core/TupleWMEAccessor.hpp>
#include <rete-core/TupleWME.hpp>
#include <rete-core/builtins/NumberToNumberConversion.hpp>

namespace sempr {

DateDayBuilder::DateDayBuilder()
    : rete::NodeBuilder("date:day", rete::NodeBuilder::BuilderType::BUILTIN)
{
}

rete::Builtin::Ptr DateDayBuilder::buildBuiltin(rete::ArgumentList& args) const
{
    // exactly 2 args: day, date
    rete::util::requireNumberOfArgs(args, 2);
    rete::util::requireUnboundVariable(args, 0);

    auto date = rete::util::requireInterpretation<std::string>(args, 1);
    
    // create the node
    auto node = std::make_shared<DateDayNode>(std::move(date));

    // bind the result variable to an accessor matching the return of DateDayNode
    auto resultAccessor = std::make_shared<rete::TupleWME<int>::Accessor<0>>();

    args[0].bind(resultAccessor);

    return node;
}


}
