#ifndef SEMPR_GEOSPLUGIN_HPP_
#define SEMPR_GEOSPLUGIN_HPP_

#include <QObject>
#include <QtPlugin>


#include "../CapabilityInterface.hpp"

namespace sempr {

class GeosPlugin : public CapabilityInterface
{
public:
    void setup(Core *core) const override;
};



class  GeosPluginBuilder : public QObject, public CapabilityBuilderInterface {
    Q_OBJECT
#ifndef QT4
    Q_PLUGIN_METADATA(IID "sempr.CapabilityBuilderInterface/1.0")
#endif
    Q_INTERFACES(sempr::CapabilityBuilderInterface)
public:
    CapabilityInterface* create() const override;
};

}

#endif /* include guard: SEMPR_GEOSPLUGIN_HPP_ */
