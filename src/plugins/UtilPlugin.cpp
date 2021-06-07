#include "plugins/UtilPlugin.hpp"

#include "Core.hpp"

#include "component/TextComponent.hpp"

#include "nodes/InferECBuilder.hpp"
#include "nodes/ECNodeBuilder.hpp"
#include "nodes/ConstructRulesBuilder.hpp"
#include "nodes/FileMonitorNodeBuilder.hpp"
#include "nodes/TextComponentTextBuilder.hpp"
#include "nodes/CreateEntityBuilder.hpp"

namespace sempr {

void UtilPlugin::setup(Core* core) const
{
    core->parser().registerNodeBuilder<ECNodeBuilder<Component>>();
    core->parser().registerNodeBuilder<ECNodeBuilder<TextComponent>>();
    core->parser().registerNodeBuilder<InferECBuilder>();
    core->parser().registerNodeBuilder<ConstructRulesBuilder>(core);
    core->parser().registerNodeBuilder<FileMonitorNodeBuilder>(core->reasonerMutex());
    core->parser().registerNodeBuilder<TextComponentTextBuilder>();
    core->parser().registerNodeBuilder<CreateEntityBuilder>();

    // default rule to infer rules from entities with text components
    core->addRules(
        "[inferRules:\n"
        "   (?e <type> <Rules>),\n"
        "   EC<TextComponent>(?e ?c),\n"
        "   text:value(?text ?c)\n"
        " -> \n"
        "   constructRules(?text)]"
    );
}

CapabilityInterface* UtilPluginBuilder::create() const
{
    return new UtilPlugin();
}

#ifdef QT4
Q_EXPORT_PLUGIN2(utilities, UtilPluginBuilder)
#endif

}
