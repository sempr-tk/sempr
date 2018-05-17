#include <sempr/processing/SopranoModule.hpp>
#include <sempr/core/EntityEvent.hpp>
#include <Soprano/Inference/RuleParser>
#include <Soprano/Inference/Rule>

namespace sempr { namespace processing {

SopranoModule::SopranoModule()
{
    model_ = Soprano::createModel();
    infmodel_ = new Soprano::Inference::InferenceModel(model_);
}

SopranoModule::~SopranoModule()
{
    delete infmodel_;
    delete model_;
}


void SopranoModule::process(core::EntityEvent<entity::RDFEntity>::Ptr event)
{
    dirty_ = true;

    // remove all triples in the model that are associated with this entity
    model_->removeAllStatements(
        Soprano::Node(), Soprano::Node(), Soprano::Node(),
        Soprano::Node::createResourceNode(
            QUrl(
                (sempr::baseURI() + event->getEntity()->id()).c_str()
            )
        )
    );

    // if the entity was removed from the database we are finished now.
    if (event->what() == event->REMOVED) return;

    // add all triples of the entity
    auto entity = event->getEntity();
    for (auto t : *entity) {
        Soprano::Node s = Soprano::Node::fromN3( QString::fromStdString(t.subject) );
        Soprano::Node p = Soprano::Node::fromN3( QString::fromStdString(t.predicate) );
        Soprano::Node o = Soprano::Node::fromN3( QString::fromStdString(t.object) );
        // TODO -- the soprano module uses the entity's id directly for the 4th part in the "triple",
        // so this could/should be removed from the Triple-class and RDFEntity-stuff?
        Soprano::Node d = Soprano::Node::createResourceNode(
            QUrl( (sempr::baseURI() + entity->id()).c_str() )
        );

        model_->addStatement(s, p, o, d);
    }
}

void SopranoModule::process(core::EntityEvent<entity::RuleSet>::Ptr event)
{
    dirty_ = true;
    auto entity = event->getEntity();

    // remove old rules
    ruleMap_.erase(entity->id());

    if (event->what() != event->REMOVED) {
        // not removed --> added or changed, add rules again
        // setup parser with prefixes
        Soprano::Inference::RuleParser parser;
        for (auto pre : entity->prefixes) {
            parser.addPrefix(QString::fromStdString(pre.first), QUrl(QString::fromStdString(pre.second)));
        }
        // parse rules and add to map
        for (auto rule : entity->rules) {
            ruleMap_[entity->id()].push_back(parser.parseRule(QString::fromStdString(rule)));
        }
    }

    // soprano does not support removing single rules, so we have to set them as a whole.
    QList<Soprano::Inference::Rule> sopRules;
    for (auto e : ruleMap_) {
        for (auto r : e.second) {
            sopRules.append(r);
        }
    }
    infmodel_->setRules(sopRules);
}




void SopranoModule::process(query::SPARQLQuery::Ptr query)
{
    if (dirty_) {
        infmodel_->performInference();
        // TODO: this always performs inference on the whole model, which is not the best idea.
        // pro: easy to implement
        // pro: only performs inference when a sparql query is to be answered
        // con: inference on whole model
        // alternative:
        //      - update triple in infmodel instead of model
        //      - only set dirty (for performInference) when a ruleset is changed
    }

    QString sparql = QString::fromStdString(query->toString());
    // std::cout << sparql.toStdString() << '\n';
    auto results = this->model_->executeQuery(sparql, Soprano::Query::QueryLanguageSparql);

    while (results.next())
    {
        std::map<std::string, std::string> bindings;
        auto bset = results.currentBindings();
        QStringList names = bset.bindingNames();
        for (int i = 0; i < names.size(); i++) {
            bindings[names[i].toStdString()] = bset[names[i]].toString().toStdString();
        }

        query->results.push_back(bindings);
    }
}


}}
