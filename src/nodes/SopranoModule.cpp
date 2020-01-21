#include <iostream>

#include "nodes/SopranoModule.hpp"
#include "RDF.hpp"

namespace sempr {

// -------------------------------------------------------------------
// SopranoModule
// -------------------------------------------------------------------
SopranoModule::SopranoModule()
    : model_(Soprano::createModel())
{
}

SopranoModule::~SopranoModule()
{
    delete model_;
}

Soprano::Statement SopranoModule::StatementFromSPO(
        const std::string& subject,
        const std::string& predicate,
        const std::string& object)
{
    auto nSub = Soprano::Node::fromN3( QString::fromStdString(subject) );
    auto nPred = Soprano::Node::fromN3( QString::fromStdString(predicate) );
    auto nObj = Soprano::Node::fromN3( QString::fromStdString(object) );

    Soprano::Statement statement(nSub, nPred, nObj);
    return statement;
}

void SopranoModule::addTriple(const std::string& s,
                              const std::string& p,
                              const std::string& o)
{
    auto statement = StatementFromSPO(s, p, o);
    if (!statement.isValid())
    {
        std::cerr << "SopranoModule: Invalid triple ("
                  << s << " " << p << " " << o << " )" << std::endl;
    }
    else
    {
        model_->addStatement(statement);
    }
}

void SopranoModule::removeTriple(const std::string& s,
                                 const std::string& p,
                                 const std::string& o)
{
    auto statement = StatementFromSPO(s, p, o);
    model_->removeStatement(statement);
}


void SopranoModule::answer(SPARQLQuery& query) const
{
    QString q = QString::fromStdString(query.toString());

    auto results = model_->executeQuery(q, Soprano::Query::QueryLanguageSparql);

    while (results.next())
    {
        SPARQLQuery::Bindings bindings;
        auto bset = results.currentBindings();
        QStringList names = bset.bindingNames();
        for (int i = 0; i < names.size(); i++)
        {
            SPARQLQuery::Value val;
            const auto& b = bset[names[i]];

            // add the type of the bound value
            if (b.isResource())     val.first = SPARQLQuery::RESOURCE;
            else if (b.isLiteral()) val.first = SPARQLQuery::LITERAL;
            else if (b.isBlank())   val.first = SPARQLQuery::BLANK;

            // add bound value
            val.second = b.toString().toStdString();

            // add binding to result
            bindings[names[i].toStdString()] = val;
        }
        // add result to result vector
        query.results.push_back(bindings);
    }
}


// -------------------------------------------------------------------
// SopranoNode
// -------------------------------------------------------------------
SopranoNode::SopranoNode(
        SopranoModule::Ptr module,
        std::unique_ptr<rete::StringAccessor> sub,
        std::unique_ptr<rete::StringAccessor> pred,
        std::unique_ptr<rete::StringAccessor> obj)
    : module_(module),
      sub_(std::move(sub)), pred_(std::move(pred)), obj_(std::move(obj))
{
}

void SopranoNode::execute(rete::Token::Ptr token, rete::PropagationFlag flag,
        std::vector<rete::WME::Ptr>&)
{
    // get s,p,o from the token
    std::string s, p, o;
    sub_->getValue(token, s);
    pred_->getValue(token, p);
    obj_->getValue(token, o);

    if (flag == rete::PropagationFlag::ASSERT)
    {
        module_->addTriple(s, p, o);
    }
    else if (flag == rete::PropagationFlag::RETRACT)
    {
        module_->removeTriple(s, p, o);
    }
    else
    {
        // UPDATE. what to do? the triple already changed, we'd need a
        // token -> triple matching to know what to retract. But fortunately
        // triples are immutable and never change, right? RIGHT?
        throw std::exception();
    }
}

std::string SopranoNode::toString() const
{
    return "SopranoNode";
}

// -------------------------------------------------------------------
// SPARQLQuery
// -------------------------------------------------------------------
SPARQLQuery::SPARQLQuery()
{
    prefixes["rdf"] = sempr::rdf::baseURI();
    prefixes["rdfs"] = sempr::rdfs::baseURI();
    prefixes["owl"] = sempr::owl::baseURI();
    prefixes["sempr"] = sempr::baseURI();
    prefixes["xsd"] = sempr::xsd::baseURI();
}

std::string SPARQLQuery::toString() const
{
    std::string q = "";
    for (auto p : this->prefixes)
    {
        q += "PREFIX " + p.first + ": <" + p.second + "> \n";
    }
    q += this->query;
    return q;
}

}
