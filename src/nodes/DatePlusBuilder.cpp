#include "nodes/DatePlusBuilder.hpp"
#include "nodes/DatePlusNode.hpp"

#include <rete-core/TupleWMEAccessor.hpp>
#include <rete-core/TupleWME.hpp>
#include <rete-core/builtins/NumberToNumberConversion.hpp>

namespace sempr {

DatePlusBuilder::DatePlusBuilder()
    : rete::NodeBuilder("date:plus", rete::NodeBuilder::BuilderType::BUILTIN)
{
}

rete::Builtin::Ptr DatePlusBuilder::buildBuiltin(rete::ArgumentList& args) const
{
    // exactly 3 args: newDate, date, days
    rete::util::requireNumberOfArgs(args, 3);
    rete::util::requireUnboundVariable(args, 0);

    auto date = rete::util::requireInterpretation<std::string>(args, 1);
    auto days = rete::util::requireInterpretation<int>(args, 2);
    
    // create the node
    auto node = std::make_shared<DatePlusNode>(std::move(date), std::move(days));

    // bind the result variable to an accessor matching the return of DateDiffNode
    auto resultAccessor = std::make_shared<rete::TupleWME<std::string>::Accessor<0>>();
   
    args[0].bind(resultAccessor);
    
    return node;
}


}
