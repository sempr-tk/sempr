#ifndef SEMPR_AFFINETFPLUGIN_HPP_
#define SEMPR_AFFINETFPLUGIN_HPP_

#include <QObject>
#include <QtPlugin>

#include "../CapabilityInterface.hpp"

namespace sempr {

class AffineTFPlugin : public QObject, public CapabilityInterface {
    Q_OBJECT
#ifndef QT4
    Q_PLUGIN_METADATA(IID "sempr.CapabilityInterface/1.0")
#endif
    Q_INTERFACES(sempr::CapabilityInterface)

public:
    void setup(Core* core) const override;
};

}

#endif /* include guard: SEMPR_AFFINETFPLUGIN_HPP_ */
