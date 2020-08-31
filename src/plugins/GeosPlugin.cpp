#include "plugins/GeosPlugin.hpp"

#include "Core.hpp"
#include "component/GeosGeometry.hpp"
#include "nodes/ECNodeBuilder.hpp"
#include "nodes/InferECBuilder.hpp"

#include "nodes/GeoDistanceBuilder.hpp"
#include "nodes/GeoConversionBuilders.hpp"

namespace sempr {

void GeosPlugin::setup(Core* core) const
{
    core->parser().registerNodeBuilder<ECNodeBuilder<GeosGeometryInterface>>();
    core->parser().registerNodeBuilder<ECNodeBuilder<GeosGeometry>>();

    core->parser().registerNodeBuilder<GeoDistanceBuilder>();
    core->parser().registerNodeBuilder<UTMFromWGSBuilder>();
}

#ifdef QT4
Q_EXPORT_PLUGIN2(geosgeometries, GeosPlugin)
#endif

}
