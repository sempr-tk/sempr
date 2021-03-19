#ifndef SEMPR_RDFPLUGIN_HPP_
#define SEMPR_RDFPLUGIN_HPP_

#include <QObject>
#include <QtPlugin>

#include <rete-reasoner/ParsedRule.hpp>

#include "../CapabilityInterface.hpp"
#include "../nodes/SopranoModule.hpp"
#include "../nodes/ComponentQuery.hpp"

namespace sempr {

/**
    Registers rdf utilities at the rule parser.
    ExtractTriples, LoadTriplesFromFile, ...
*/
class RDFPlugin : public QObject, public CapabilityInterface {
    Q_OBJECT
#ifndef QT4
    Q_PLUGIN_METADATA(IID "sempr.CapabilityInterface/1.0")
#endif
    Q_INTERFACES(sempr::CapabilityInterface)

    SopranoModule::Ptr soprano_;
    ComponentQueryModule::Ptr component_;

    // the rule to update the component query module is not inserted through
    // the usual means of the rete core, and hence needs to be kept alive here
    mutable std::vector<rete::ParsedRule::Ptr> rulesKeepAlive_;
public:
    RDFPlugin();

    void setup(Core* core) const override;
    SopranoModule& soprano() const;
    ComponentQuery<> componentQuery(const std::string& sparql) const;
};

}

#endif /* include guard: SEMPR_RDFPLUGIN_HPP_ */

