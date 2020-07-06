#include "plugins/GeosPlugin.hpp"

#include "component/GeosGeometry.hpp"
#include "nodes/GeoDistanceBuilder.hpp"
#include "nodes/GeoConversionBuilders.hpp"

namespace sempr {

void GeosPlugin::setup(Core* core) const
{
    core->parser().registerNodeBuilder<GeoDistanceBuilder>();
    core->parser().registerNodeBuilder<UTMFromWGSBuilder>();
}


}
