#ifndef SEMPR_CORE_IDGENERATION_STRATEGY_
#define SEMPR_CORE_IDGENERATION_STRATEGY_

#include <string>

namespace sempr { namespace core {

/**
    The IDGenerationStrategy is the base class for different implementations
    of generating object-ids.
*/
class IDGenerationStrategy {
public:
    virtual ~IDGenerationStrategy() {};
    /**
        Generate a new id, based on a given prefix.
    */
    virtual std::string generate(const std::string& prefix) = 0;
    /**
        Revoke an existing id, freeing it for further use.
        Also takes the prefix it has been created from as an input.
    */
    virtual void revoke(const std::string& prefix, const std::string& id) = 0;
};

}}


#endif /* end of include guard: SEMPR_CORE_IDGENERATION_STRATEGY_ */
