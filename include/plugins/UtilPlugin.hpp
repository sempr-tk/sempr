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
class UtilPlugin : public QObject, public CapabilityInterface {
    Q_OBJECT
#ifndef QT4
    Q_PLUGIN_METADATA(IID "sempr.CapabilityInterface/1.0")
#endif
    Q_INTERFACES(sempr::CapabilityInterface)

public:
    void setup(Core* core) const override;
};

}

#endif /* include guard: SEMPR_UTILPLUGIN_HPP_ */
