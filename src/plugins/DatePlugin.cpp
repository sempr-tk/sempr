#include "plugins/DatePlugin.hpp"
#include "Core.hpp"
#include "nodes/DateMonthBuilder.hpp"
#include "nodes/DateYearBuilder.hpp"
#include "nodes/DateDayBuilder.hpp"
#include "nodes/DateDiffBuilder.hpp"
//Add further builtins...

namespace sempr {

void DatePlugin::setup(Core* core) const
{
    core->parser().registerNodeBuilder<DateMonthBuilder>();    
    core->parser().registerNodeBuilder<DateYearBuilder>();
    core->parser().registerNodeBuilder<DateDayBuilder>();
    core->parser().registerNodeBuilder<DateDiffBuilder>();
    //core->parser().registerNodeBuilder<DateLTBuilder>();
    //core->parser().registerNodeBuilder<DatePlusBuilder>();
    //core->parser().registerNodeBuilder<DateMinusBuilder>();
    //...
}

CapabilityInterface* DatePluginBuilder::create() const
{
    return new DatePlugin();
}

#ifdef QT4
Q_EXPORT_PLUGIN2(date, DatePluginBuilder)
#endif

}
