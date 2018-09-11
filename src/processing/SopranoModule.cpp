#include <sempr/processing/SopranoModule.hpp>
#include <sempr/core/EntityEvent.hpp>
#include <Soprano/Inference/RuleParser>
#include <Soprano/Inference/Rule>

namespace sempr { namespace processing {

SopranoModule::SopranoModule()
{
    reasoningEnabled_ = false;
    model_ = Soprano::createModel();
    infmodel_ = new Soprano::Inference::InferenceModel(model_);
}

SopranoModule::~SopranoModule()
{
    delete infmodel_;
    delete model_;
}


void SopranoModule::enableReasoning(bool reason)
{
    reasoningEnabled_ = reason;
}

void SopranoModule::process(core::EntityEvent<entity::RDFEntity>::Ptr event)
{
    Soprano::Model* model;
    if (reasoningEnabled_)
    {
        model = infmodel_;
    }
    else
    {
        model = model_;
        dirty_ = true;
    }
    // dirty#_ = true;

    // remove all triples in the model that are associated with this entity
    model->removeAllStatements(
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

        Soprano::Statement st(s, p, o, d);
        if (!st.isValid())
        {
            std::cout << "SopranoModule: Cannot add invalid statement ("
                      << st.subject().toString().toStdString() << ","
                      << st.predicate().toString().toStdString() << ","
                      << st.object().toString().toStdString() << ") from "
                      << entity->id() << std::endl;
        } else {
            model->addStatement(st);
        }
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
    if (dirty_ && reasoningEnabled_) {
        infmodel_->performInference();
        dirty_ = false;
        // TODO: this always performs inference on the whole model, which is not the best idea.
        // pro: easy to implement
        // pro: only performs inference when a sparql query is to be answered
        // con: inference on whole model
        // alternative:
        //      - update triple in infmodel instead of model
        //      - only set dirty (for performInference) when a ruleset is changed
    }

    Soprano::Model* model = (reasoningEnabled_ ? infmodel_ : model_);

    QString sparql = QString::fromStdString(query->toString());
    // std::cout << sparql.toStdString() << '\n';
    auto results = model->executeQuery(sparql, Soprano::Query::QueryLanguageSparql);

    while (results.next())
    {
        query::SPARQLQuery::Bindings bindings;

        auto bset = results.currentBindings();
        QStringList names = bset.bindingNames();
        for (int i = 0; i < names.size(); i++) {
            query::SPARQLQuery::Value val;

            const auto& b = bset[names[i]];

            // add type of the bound value
            if (b.isResource())     val.first = query::SPARQLQuery::RESOURCE;
            else if (b.isLiteral()) val.first = query::SPARQLQuery::LITERAL;
            else if (b.isBlank())   val.first = query::SPARQLQuery::BLANK;

            // add bound value
            val.second = b.toString().toStdString();

            // add binding to result
            bindings[names[i].toStdString()] = val;
        }

        // ad result to list of results
        query->results.push_back(bindings);
    }
}


}}
