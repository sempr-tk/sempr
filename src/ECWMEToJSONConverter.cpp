#include "ECWMEToJSONConverter.hpp"
#include "ECWME.hpp"

#include <nlohmann/json.hpp>
#include <cereal/archives/json.hpp>
#include <rete-core/Util.hpp>

namespace sempr {

bool ECWMEToJSONConverter::convert(rete::WME::Ptr wme, std::string& json)
{
    auto ecwme = std::dynamic_pointer_cast<ECWME>(wme);
    if (!ecwme) return false;

    auto entity = std::get<0>(ecwme->value_);
    auto component = std::get<1>(ecwme->value_);
    auto tag = std::get<2>(ecwme->value_);

    nlohmann::json j;
    j["type"] = "Entity-Component-Tag";
    j["description"] = ecwme->description_;
    j["value"]["entity"]["id"] = entity->id();
    j["value"]["entity"]["idIsURI"] = entity->idIsURI();

    std::stringstream ssComponent;
    {
        cereal::JSONOutputArchive ar(ssComponent);
        component->saveToJSON(ar);
    }
    auto compJson = nlohmann::json::parse(ssComponent.str());
    j["value"]["component"] = compJson["value0"];
    j["value"]["component-type"] = rete::util::demangle(typeid(*component).name());
    j["value"]["tag"] = tag;

    json = j.dump();
    return true;
}


}