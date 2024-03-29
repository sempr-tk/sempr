#include "plugins/GeosPlugin.hpp"

#include "Core.hpp"
#include "component/GeosGeometry.hpp"
#include "nodes/ECNodeBuilder.hpp"
#include "nodes/InferECBuilder.hpp"

#include "nodes/GeoDistanceBuilder.hpp"
#include "nodes/GeoConversionBuilders.hpp"
#include "nodes/GeoAreaBuilder.hpp"
#include "nodes/GeoIntersectionBuilder.hpp"
#include "nodes/GeoUnionBuilder.hpp"
#include "nodes/GeoUnionAllBuilder.hpp"
#include "nodes/GeoIntersectsBuilder.hpp"
#include "nodes/GeoBufferBuilder.hpp"
#include "nodes/GeoDifferenceBuilder.hpp"
#include "nodes/GeoFromWKTBuilder.hpp"
#include "nodes/GeoToWKTBuilder.hpp"

namespace sempr {

void GeosPlugin::setup(Core* core) const
{
    core->parser().registerNodeBuilder<ECNodeBuilder<GeosGeometryInterface>>();
    core->parser().registerNodeBuilder<ECNodeBuilder<GeosGeometry>>();

    core->parser().registerNodeBuilder<GeoDistanceBuilder>();
    core->parser().registerNodeBuilder<UTMFromWGSBuilder>();
    core->parser().registerNodeBuilder<WGSFromUTMBuilder>();
    core->parser().registerNodeBuilder<GeoAreaBuilder>();
    core->parser().registerNodeBuilder<GeoIntersectionBuilder>();
    core->parser().registerNodeBuilder<GeoUnionBuilder>();
    core->parser().registerNodeBuilder<GeoUnionAllBuilder>();
    core->parser().registerNodeBuilder<GeoIntersectsBuilder>();
    core->parser().registerNodeBuilder<GeoBufferBuilder>();
    core->parser().registerNodeBuilder<GeoDifferenceBuilder>();
    core->parser().registerNodeBuilder<GeoFromWKTBuilder>();
    core->parser().registerNodeBuilder<GeoToWKTBuilder>();
}


CapabilityInterface* GeosPluginBuilder::create() const
{
    return new GeosPlugin();
}

#ifdef QT4
Q_EXPORT_PLUGIN2(geosgeometries, GeosPluginBuilder)
#endif

}
