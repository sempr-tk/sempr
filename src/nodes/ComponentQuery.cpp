#include "nodes/ComponentQueryNode.hpp"
#include "RDF.hpp"

namespace sempr {

    namespace {
        std::string wmeToId(ECWME::Ptr wme)
        {
            auto entity = std::get<0>(wme->value_);
            std::string id = entity->id();
            if (!entity->idIsURI())
            {
                id = sempr::baseURI() + id;
                // still an id, not a uri -- missing <>
                // because those just mark it as a resource,
                // which is done in the sparql result in a
                // separate tag
            }
            else
            {
                // for the same reason, trim <> from URI ids
                if (id.size() > 2 && (*id.begin()) == '<' && (*id.rbegin()) == '>')
                {
                    id = id.substr(1, id.size()-2);
                }
            }

            return id;
        }
    }

void ComponentQueryModule::addECWME(ECWME::Ptr wme)
{
    std::string id = wmeToId(wme);
    ecwmes_[id].insert(wme);
}

void ComponentQueryModule::removeECWME(ECWME::Ptr wme)
{
    std::string id = wmeToId(wme);
    ecwmes_[id].erase(wme);
}

std::set<ECWME::Ptr> ComponentQueryModule::get(const std::string& id)
{
    return ecwmes_[id];
}


// --------------------------------------------------------------------

ComponentQueryNode::ComponentQueryNode(ComponentQueryModule::Ptr mod)
    : module_(mod)
{
}

std::string ComponentQueryNode::toString() const
{
    return "ComponentQueryNode";
}

void ComponentQueryNode::execute(
        rete::Token::Ptr token, rete::PropagationFlag flag,
        std::vector<rete::WME::Ptr>&)
{
    ECWME::Ptr ecwme = nullptr;

    if (token)
    {
        auto wme = token->wme;
        if (wme)
        {
            ecwme = std::static_pointer_cast<ECWME>(wme);
        }
    }

    if (!ecwme) return;


    switch (flag) {
        case rete::PropagationFlag::ASSERT:
            module_->addECWME(ecwme);
            break;
        case rete::PropagationFlag::RETRACT:
            module_->removeECWME(ecwme);
            break;
        case rete::PropagationFlag::UPDATE:
            // something internally changed - so what?
            break;
    }
}


}
