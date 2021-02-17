#include <rete-core/TupleWME.hpp>

#include "Core.hpp"
#include "nodes/CreateEntityNode.hpp"
#include "nodes/TempEntityPool.hpp"

namespace sempr {

CreateEntityNode::CreateEntityNode(
        rete::PersistentInterpretation<std::string> id)
    :
        rete::Builtin("createEntity"),
        id_(std::move(id))
{
}


rete::WME::Ptr CreateEntityNode::process(rete::Token::Ptr token)
{
    std::string id;
    id_.interpretation->getValue(token, id);

    auto entity = TempEntityPool::instance().get(id);
    auto wme = std::make_shared<rete::TupleWME<Entity::Ptr>>(entity);
    return wme;
}

bool CreateEntityNode::operator==(const rete::BetaNode& other) const
{
    auto o = dynamic_cast<const CreateEntityNode*>(&other);
    if (!o) return false;

    return *(o->id_.accessor) == *(this->id_.accessor);
}


}
