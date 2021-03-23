#include "plugins/AffineTFPlugin.hpp"

#include "Core.hpp"
#include "nodes/ECNodeBuilder.hpp"
#include "nodes/InferECBuilder.hpp"

#include "component/AffineTransform.hpp"
#include "nodes/AffineTransformBuilders.hpp"

namespace sempr {

void AffineTFPlugin::setup(Core* core) const
{
    core->parser().registerNodeBuilder<ECNodeBuilder<AffineTransform>>();
    core->parser().registerNodeBuilder<AffineTransformGetBuilder>();
    core->parser().registerNodeBuilder<AffineTransformInvBuilder>();
    core->parser().registerNodeBuilder<AffineTransformMulBuilder>();
    core->parser().registerNodeBuilder<AffineTransformCreateBuilder>();
}

CapabilityInterface* AffineTFPluginBuilder::create() const
{
    return new AffineTFPlugin();
}

#ifdef QT4
Q_EXPORT_PLUGIN2(affinetransforms, AffineTFPluginBuilder)
#endif

}
