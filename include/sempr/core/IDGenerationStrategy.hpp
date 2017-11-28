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
    virtual std::string generate(const std::string& prefix) = 0;
};

}}


#endif /* end of include guard: SEMPR_CORE_IDGENERATION_STRATEGY_ */
