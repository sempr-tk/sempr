#include <sempr/processing/SopranoModule.hpp>
#include <sempr/core/EntityEvent.hpp>

namespace sempr { namespace processing {

SopranoModule::SopranoModule()
{
    model_ = Soprano::createModel();
    // auto infmodel = new Soprano::Inference::InferenceModel(model_);

    addOverload<core::EntityEvent<entity::RDFEntity> >(
        [this](core::EntityEvent<entity::RDFEntity>::Ptr e) {
            this->process(e);
        }
    );
}

SopranoModule::~SopranoModule()
{
    // delete infmodel_;
    delete model_;
}

void SopranoModule::test()
{
    QString query = "SELECT * WHERE { ?s ?p ?o .}";
    auto resultIter = model_->executeQuery(query, Soprano::Query::QueryLanguageSparql);

    size_t cnt = 0;
    while (resultIter.next())
    {
        std::cout << "Result " << ++cnt << ": ";
        auto bset = resultIter.currentBindings();
        QStringList names = bset.bindingNames();
        for (size_t i = 0; i < names.size(); i++) {
            QString key = names[i];
            QString value = bset[key].toString();
            std::cout << key.toStdString() << " = " << value.toStdString() << " | ";
        }
        std::cout << '\n';
    }
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
