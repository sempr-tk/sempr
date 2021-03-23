#ifndef SEMPR_UTILPLUGIN_HPP_
#define SEMPR_UTILPLUGIN_HPP_

#include <QObject>
#include <QtPlugin>

#include "../CapabilityInterface.hpp"

namespace sempr {

/**
    This plugin registers a bunch of utility functionality that does not fit
    a specific group. TextComponent, FileMonitor, etc.
*/
class UtilPlugin : public CapabilityInterface {
public:
    void setup(Core* core) const override;
};

class UtilPluginBuilder : public QObject, public CapabilityBuilderInterface {
    Q_OBJECT
#ifndef QT4
    Q_PLUGIN_METADATA(IID "sempr.CapabilityBuilderInterface/1.0")
#endif
    Q_INTERFACES(sempr::CapabilityBuilderInterface)
public:
    CapabilityInterface* create() const override;
};

}

#endif /* include guard: SEMPR_UTILPLUGIN_HPP_ */
