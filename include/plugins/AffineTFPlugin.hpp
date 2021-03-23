#ifndef SEMPR_AFFINETFPLUGIN_HPP_
#define SEMPR_AFFINETFPLUGIN_HPP_

#include <QObject>
#include <QtPlugin>

#include "../CapabilityInterface.hpp"

namespace sempr {

class AffineTFPlugin : public CapabilityInterface {
public:
    void setup(Core* core) const override;
};



class AffineTFPluginBuilder : public QObject, public CapabilityBuilderInterface {
    Q_OBJECT
#ifndef QT4
    Q_PLUGIN_METADATA(IID "sempr.CapabilityBuilderInterface/1.0")
#endif
    Q_INTERFACES(sempr::CapabilityBuilderInterface)
public:
    CapabilityInterface* create() const override;
};

}

#endif /* include guard: SEMPR_AFFINETFPLUGIN_HPP_ */
