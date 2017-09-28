#include <sempr/core/RDF.h>
#include <sempr/entity/Entity.h>
#include <sempr/core/EventBroker.h>

namespace sempr { namespace entity {

class RDFEntityI {
public:    
    virtual void getTriples(std::vector<core::Triple>& triples) const = 0;
};

}}
