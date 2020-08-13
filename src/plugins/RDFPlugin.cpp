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

namespace sempr {

RDFPlugin::RDFPlugin()
    : soprano_(new SopranoModule)
{
}


void RDFPlugin::setup(Core* core) const
{
    core->parser().registerNodeBuilder<ECNodeBuilder<TripleContainer>>();
    core->parser().registerNodeBuilder<ECNodeBuilder<TripleDocument>>();
    core->parser().registerNodeBuilder<ECNodeBuilder<TripleVector>>();
    core->parser().registerNodeBuilder<ECNodeBuilder<TriplePropertyMap>>();

    core->parser().registerNodeBuilder<InferECBuilder<TripleContainer>>();
    core->parser().registerNodeBuilder<InferECBuilder<TripleDocument>>();
    core->parser().registerNodeBuilder<InferECBuilder<TripleVector>>();
    core->parser().registerNodeBuilder<InferECBuilder<TriplePropertyMap>>();

    core->parser().registerNodeBuilder<ExtractTriplesBuilder>();
    core->parser().registerNodeBuilder<SopranoNodeBuilder>(soprano_);
    core->parser().registerNodeBuilder<TripleDocumentFilenameBuilder>();
    core->parser().registerNodeBuilder<LoadTriplesFromFileBuilder>();

    // default rules!
    core->addRules(
        "[extractTriples: EC<TripleContainer>(?e ?c) -> ExtractTriples(?c)]\n"
        "[updateSoprano:  (?s ?p ?o) -> SopranoModule(?s ?p ?o)]"
    );
}

SopranoModule& RDFPlugin::soprano() const
{
    return *soprano_;
}

#ifdef QT4
Q_EXPORT_PLUGIN2(rdf, RDFPlugin)
#endif

}
