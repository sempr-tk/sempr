#include "plugins/RDFPlugin.hpp"

#include "Core.hpp"
#include "component/TripleContainer.hpp"
#include "component/TripleVector.hpp"
#include "component/TripleDocument.hpp"
#include "component/TriplePropertyMap.hpp"

#include "nodes/ECNodeBuilder.hpp"
#include "nodes/InferECBuilder.hpp"

#include "nodes/SopranoNodeBuilder.hpp"
#include "nodes/ExtractTriplesBuilder.hpp"
#include "nodes/TripleDocumentFilenameBuilder.hpp"
#include "nodes/LoadTriplesFromFileBuilder.hpp"
#include "nodes/ComponentQueryNode.hpp"
#include "nodes/ComponentQuery.hpp"
#include "nodes/ComponentQueryBuilder.hpp"

namespace sempr {

RDFPlugin::RDFPlugin()
    : soprano_(new SopranoModule()),
      component_(new ComponentQueryModule())
{
}


void RDFPlugin::setup(Core* core) const
{
    core->parser().registerNodeBuilder<ECNodeBuilder<TripleContainer>>();
    core->parser().registerNodeBuilder<ECNodeBuilder<TripleDocument>>();
    core->parser().registerNodeBuilder<ECNodeBuilder<TripleVector>>();
    core->parser().registerNodeBuilder<ECNodeBuilder<TriplePropertyMap>>();

    core->parser().registerNodeBuilder<ExtractTriplesBuilder>();
    core->parser().registerNodeBuilder<SopranoNodeBuilder>(soprano_);
    core->parser().registerNodeBuilder<TripleDocumentFilenameBuilder>();
    core->parser().registerNodeBuilder<LoadTriplesFromFileBuilder>();
    core->parser().registerNodeBuilder<ComponentQueryBuilder>(component_);

    // default rules!
    core->addRules(
        "[extractTriples: EC<TripleContainer>(?e ?c) -> ExtractTriples(?c)]\n"
        "[updateSoprano:  (?s ?p ?o) -> SopranoModule(?s ?p ?o)]"
    );


    // this requires an ECNodeBuilder<Component>, which might be loaded by
    // another plugin, but should not be loaded by this... so instead of hoping
    // that the plugins are loaded in the correct order, just use a different
    // RuleParser and keep the resulting rule alive.
    // TODO: Does this then even show up in the sempr-gui? probably not...
    //       Maybe add a way to add the already parsed rule to the core?
    //       Or optionally use a different parser when handing a rule-string
    //       to the core?
    rete::RuleParser parser;
    parser.registerNodeBuilder<ECNodeBuilder<Component>>();
    parser.registerNodeBuilder<ComponentQueryBuilder>(component_);

    std::vector<rete::ParsedRule::Ptr> rules = parser.parseRules(
        "[updateCompQuery: EC<Component>(?e ?c) -> ComponentQueryModule()]",
        core->reasoner().net());

    rulesKeepAlive_.insert(rulesKeepAlive_.begin(), rules.begin(), rules.end());
}

SopranoModule& RDFPlugin::soprano() const
{
    return *soprano_;
}

ComponentQuery<> RDFPlugin::componentQuery(const std::string& sparql) const
{
    return ComponentQuery<>(soprano_, component_, sparql);
}


CapabilityInterface* RDFPluginBuilder::create() const
{
    return new RDFPlugin();
}

#ifdef QT4
Q_EXPORT_PLUGIN2(rdf, RDFPluginBuilder)
#endif

}
