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
    rete::util::requireNumberOfArgs(args, 3);
    rete::util::requireUnboundVariable(args, 0);

    auto date1 = rete::util::requireInterpretation<std::string>(args, 1);
    auto date2 = rete::util::requireInterpretation<std::string>(args, 2); 
    
    // create the node
    auto node = std::make_shared<DateDiffNode>(std::move(date1), std::move(date2));

    // bind the result variable to an accessor matching the return of DateDiffNode
    auto resultAccessor = std::make_shared<rete::TupleWME<int>::Accessor<0>>();
   
    args[0].bind(resultAccessor);
    
    return node;
}


}
