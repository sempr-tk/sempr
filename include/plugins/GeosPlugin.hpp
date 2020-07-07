#ifndef SEMPR_GEOSPLUGIN_HPP_
#define SEMPR_GEOSPLUGIN_HPP_

#include <QObject>
#include <QtPlugin>


#include "../CapabilityInterface.hpp"

namespace sempr {

class GeosPlugin : public QObject, public CapabilityInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "sempr.CapabilityInterface/1.0" FILE "geos.json")
    Q_INTERFACES(CapabilityInterface)

public:
    void setup(Core *core) const override;
};

}

#endif /* include guard: SEMPR_GEOSPLUGIN_HPP_ */
