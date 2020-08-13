#ifndef SEMPR_RDFPLUGIN_HPP_
#define SEMPR_RDFPLUGIN_HPP_

#include <QObject>
#include <QtPlugin>

#include "../CapabilityInterface.hpp"
#include "nodes/SopranoModule.hpp"

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
public:
    RDFPlugin();

    void setup(Core* core) const override;
    SopranoModule& soprano() const;
};

}

#endif /* include guard: SEMPR_RDFPLUGIN_HPP_ */

