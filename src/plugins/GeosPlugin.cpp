#include "plugins/GeosPlugin.hpp"
#include "../include/plugins/moc_GeosPlugin.cpp"

#include "Core.hpp"
#include "component/GeosGeometry.hpp"
#include "nodes/ECNodeBuilder.hpp"

#include "nodes/GeoDistanceBuilder.hpp"
#include "nodes/GeoConversionBuilders.hpp"

namespace sempr {

void GeosPlugin::setup(Core* core) const
{
    core->parser().registerNodeBuilder<ECNodeBuilder<GeosGeometryInterface>>();
    core->parser().registerNodeBuilder<GeoDistanceBuilder>();
    core->parser().registerNodeBuilder<UTMFromWGSBuilder>();
}


}
