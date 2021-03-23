#ifndef SEMPR_RDFPLUGIN_HPP_
#define SEMPR_RDFPLUGIN_HPP_

#include <QObject>
#include <QtPlugin>

#include "../CapabilityInterface.hpp"
#include "../nodes/SopranoModule.hpp"

namespace sempr {

/**
    Registers rdf utilities at the rule parser.
    ExtractTriples, LoadTriplesFromFile, ...
*/
class RDFPlugin : public CapabilityInterface {
    SopranoModule::Ptr soprano_;
public:
    RDFPlugin();

    void setup(Core* core) const override;
    SopranoModule& soprano() const;
};


class RDFPluginBuilder : public QObject, public CapabilityBuilderInterface {
    Q_OBJECT
#ifndef QT4
    Q_PLUGIN_METADATA(IID "sempr.CapabilityBuilderInterface/1.0")
#endif
    Q_INTERFACES(sempr::CapabilityBuilderInterface)
public:
    CapabilityInterface* create() const override;

};

}

#endif /* include guard: SEMPR_RDFPLUGIN_HPP_ */

