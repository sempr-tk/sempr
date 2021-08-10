#include "TupleWMEToJSONConverter.hpp"
#include <rete-core/TupleWME.hpp>
#include "component/AffineTransform.hpp"

#include "nlohmann/json.hpp"
#include <cereal/archives/json.hpp>
#include <rete-core/Util.hpp>
#include "Entity.hpp"
#include <rete-rdf/TriplePart.hpp>

namespace sempr {

namespace {


    template <class T>
    nlohmann::json toJson(std::shared_ptr<rete::TupleWME<T>> wme)
    {
        nlohmann::json j;
        j["type"] = rete::util::demangle(typeid(T).name());
        j["description"] = wme->description_;
        j["value"] = std::get<0>(wme->value_);
        return j;
    }

    template <class T, class = std::enable_if_t<std::is_base_of<Component, T>::value, void>>
    nlohmann::json componenttoJson(std::shared_ptr<rete::TupleWME<std::shared_ptr<T>>> wme)
    {
        Component::Ptr component = std::get<0>(wme->value_);

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


    nlohmann::json toJson(std::shared_ptr<rete::TupleWME<>> wme)
    {
        nlohmann::json j;
        j["type"] = "Empty";
        if (wme->description_.empty())
            j["description"] = "An empty data object, usually used to signal a passed check.";
        else
            j["description"] = wme->description_;

        j["value"] = {};
        return j;
    }

    nlohmann::json toJson(std::shared_ptr<rete::TupleWME<Entity::Ptr>> wme)
    {
        nlohmann::json j;
        j["type"] = "Entity";
        j["description"] = wme->description_;
        j["value"]["idIsURI"] = std::get<0>(wme->value_)->idIsURI();
        j["value"]["id"] = std::get<0>(wme->value_)->id();

        return j;
    }

    nlohmann::json toJson(std::shared_ptr<rete::TupleWME<rete::TriplePart>> wme)
    {
        nlohmann::json j;
        j["type"] = "TriplePart";
        j["description"] = wme->description_;
        j["value"] = std::get<0>(wme->value_).value;
        return j;
    }
}


bool TupleWMEToJSONConverter::convert(rete::WME::Ptr wme, std::string& json)
{
    nlohmann::json j;

    if (auto w = std::dynamic_pointer_cast<rete::TupleWME<std::string>>(wme))
        j = toJson(w);
    else if (auto w = std::dynamic_pointer_cast<rete::TupleWME<int>>(wme))
        j = toJson(w);
    else if (auto w = std::dynamic_pointer_cast<rete::TupleWME<size_t>>(wme))
        j = toJson(w);
    else if (auto w = std::dynamic_pointer_cast<rete::TupleWME<float>>(wme))
        j = toJson(w);
    else if (auto w = std::dynamic_pointer_cast<rete::TupleWME<double>>(wme))
        j = toJson(w);
    else if (auto w = std::dynamic_pointer_cast<rete::TupleWME<Entity::Ptr>>(wme))
        j = toJson(w);
    else if (auto w = std::dynamic_pointer_cast<rete::TupleWME<AffineTransform::Ptr>>(wme))
        j = componenttoJson(w);
    else if (auto w = std::dynamic_pointer_cast<rete::TupleWME<rete::TriplePart>>(wme))
        j = toJson(w);
    else if (auto w = std::dynamic_pointer_cast<rete::TupleWME<>>(wme))
        j = toJson(w);
    else
    {
        std::cout << "no match for " << rete::util::demangle(typeid(*wme).name()) << std::endl;
        return false;
    }

    json = j.dump();
    return true;
}


}
