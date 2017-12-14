#include <sempr/processing/SopranoModule.hpp>

namespace sempr { namespace processing {

SopranoModule::SopranoModule()
{
    model_ = Soprano::createModel();
    // auto infmodel = new Soprano::Inference::InferenceModel(model_);
}

SopranoModule::~SopranoModule()
{
    // delete infmodel_;
    delete model_;
}

void SopranoModule::process(core::EntityEvent<entity::RDFEntity>::Ptr event)
{
    // remove all triples in the model that are associated with this entity
    model_->removeAllStatements(
        Soprano::Node(), Soprano::Node(), Soprano::Node(),
        Soprano::Node::createResourceNode(
            QUrl(
                ("sempr://" + event->getEntity()->id()).c_str()
            )
        )
    );

    // add all triples of the entity
    auto entity = event->getEntity();
    for (auto t : *entity) {
        Soprano::Node s = Soprano::Node::fromN3( QString::fromStdString(t.subject) );
        Soprano::Node p = Soprano::Node::fromN3( QString::fromStdString(t.predicate) );
        Soprano::Node o = Soprano::Node::fromN3( QString::fromStdString(t.object) );
        Soprano::Node d = Soprano::Node::createResourceNode(
            QUrl( ("sempr://" + entity->id()).c_str() )
        );

        model_->addStatement(s, p, o, d);
    }
}


}}
