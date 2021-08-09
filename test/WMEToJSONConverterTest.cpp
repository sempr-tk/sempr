#include "Entity.hpp"
#include "ECWME.hpp"
#include "ECWMEToJSONConverter.hpp"

#include "component/AffineTransform.hpp"
#include "component/GeosGeometry.hpp"
#include "component/TriplePropertyMap.hpp"
#include <geos/geom/Point.h>

using namespace sempr;

int main()
{
    std::shared_ptr<rete::WMEToJSONConverter> conv = std::make_shared<ECWMEToJSONConverter>();

    auto entity = Entity::create();
    entity->setId("Entity_0");
    auto transform = std::make_shared<AffineTransform>();
    auto geometry = std::make_shared<GeosGeometry>();
    auto tpm = std::make_shared<TriplePropertyMap>();

    transform->setTransform(
        transform->transform() *
        Eigen::Translation3d(1, 2, 3)
    );

    geometry->setGeometry(
        geos::geom::GeometryFactory::getDefaultInstance()->createPoint({4, 5, 6})
    );

    tpm->map_["foo"] = "bar";
    tpm->map_["baz"] = {"zap:zippedy", true};
    tpm->map_["oof"] = 3.14159f;


    auto wmeTransform = std::make_shared<ECWME>(entity, transform, "transform-tag");
    auto wmeGeometry = std::make_shared<ECWME>(entity, geometry, "geometry-tag");
    auto wmeTPM = std::make_shared<ECWME>(entity, tpm, "tmp-tag");

    std::string resultJson;

    if (!conv->convert(wmeTransform, resultJson)) return 1;
    std::cout << resultJson << std::endl;

    if (!conv->convert(wmeGeometry, resultJson)) return 2;
    std::cout << resultJson << std::endl;

    if (!conv->convert(wmeTPM, resultJson)) return 3;
    std::cout << resultJson << std::endl;

    return 0;
}