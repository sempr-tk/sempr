#include "TupleGeoToJSONConverter.hpp"
#include <rete-core/TupleWME.hpp>
#include "component/GeosGeometry.hpp"

#include "nlohmann/json.hpp"
#include <cereal/archives/json.hpp>
#include <rete-core/Util.hpp>

namespace sempr {

namespace {

    template <class T, class = std::enable_if_t<std::is_base_of<Component, T>::value, void>>
    nlohmann::json toJson(std::shared_ptr<rete::TupleWME<std::shared_ptr<T>>> wme)
    {
        auto component = std::get<0>(wme->value_);

        nlohmann::json j;
        j["type"] = "Component";
        j["description"] = wme->description_;
        std::stringstream ssComponent;
        {
            cereal::JSONOutputArchive ar(ssComponent);
            component->saveToJSON(ar);
        }
        auto compJson = nlohmann::json::parse(ssComponent.str());
        j["value"]["component"] = compJson["value0"];
        j["value"]["component-type"] = rete::util::demangle(typeid(*component).name());

        return j;
    }
}


bool TupleGeoToJSONConverter::convert(rete::WME::Ptr wme, std::string& json)
{
    nlohmann::json j;

    if (auto w = std::dynamic_pointer_cast<rete::TupleWME<GeosGeometry::Ptr>>(wme))
        j = toJson(w);
    if (auto w = std::dynamic_pointer_cast<rete::TupleWME<GeosGeometryInterface::Ptr>>(wme))
        j = toJson(w);
    else
    {
        return false;
    }

    json = j.dump();
    return true;
}


}
