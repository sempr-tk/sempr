#ifndef SEMPR_CORE_OBSERVABLE_HPP_
#define SEMPR_CORE_OBSERVABLE_HPP_

#include <memory>

namespace sempr { namespace core {

class Observable {
public:
    using Ptr = std::shared_ptr<Observable>;
    virtual ~Observable() {}
};

}}

#endif /* end of include guard SEMPR_CORE_OBSERVABLE_HPP_ */
