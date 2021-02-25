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

CreateEntityNode::CreateEntityNode(
        rete::PersistentInterpretation<rete::TriplePart> uri)
    :
        rete::Builtin("createEnity"),
        uri_(std::move(uri))
{
}

rete::WME::Ptr CreateEntityNode::process(rete::Token::Ptr token)
{

    std::string id;
    bool isURI = false;

    if (id_)
    {
        id_.interpretation->getValue(token, id);
    }
    else /* uri_ */
    {
        rete::TriplePart uri;
        uri_.interpretation->getValue(token, uri);
        id = uri.value;
        isURI = true;
    }

    auto entity = TempEntityPool::instance().get(id, isURI);
    auto wme = std::make_shared<rete::TupleWME<Entity::Ptr>>(entity);
    return wme;
}

bool CreateEntityNode::operator==(const rete::BetaNode& other) const
{
    auto o = dynamic_cast<const CreateEntityNode*>(&other);
    if (!o) return false;

    if (o->id_ && this->id_)
        return *(o->id_.accessor) == *(this->id_.accessor);
    else if (o->uri_ && this->uri_)
        return *(o->uri_.accessor) == *(this->uri_.accessor);
    else
        return false;
}


}
