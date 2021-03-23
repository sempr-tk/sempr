#include "plugins/DatePlugin.hpp"
#include "Core.hpp"
#include "nodes/DateMonthBuilder.hpp"
//Add further builtins?

namespace sempr {

void DatePlugin::setup(Core* core) const
{
    core->parser().registerNodeBuilder<DateMonthBuilder>();
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
