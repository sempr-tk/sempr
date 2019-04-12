#ifndef INFLUENCEMODULE_H
#define INFLUENCEMODULE_H

#include <sempr/processing/Module.hpp>
#include <sempr/query/ObjectQuery.hpp>
#include <sempr/query/InfluenceQuery.hpp>
#include <QObject>

namespace sempr { namespace processing {

class InfluenceModule : public Module< query::InfluenceQuery >
{
public:
    InfluenceModule();
};

}}

#endif // INFLUENCEMODULE_H
