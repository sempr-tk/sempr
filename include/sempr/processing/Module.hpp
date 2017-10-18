#ifndef SEMPR_PROCESSING_MODULE_H_
#define SEMPR_PROCESSING_MODULE_H_

#include <string>
#include <memory>
#include <sempr/core/Event.hpp>
#include <sempr/core/Observer.hpp>
#include <sempr/core/EntityEvent.hpp>

namespace sempr { namespace processing {
    
class Module : public core::Observer{
public:
    using Ptr = std::shared_ptr<Module>;
    virtual std::string type() const;
    virtual ~Module(){}
    
    /** overriden from observer **/
    virtual void process(core::Event::Ptr e) override {};
};
    
} /* processing */
    
} /* sempr */

#endif /* end of include guard: SEMPR_PROCESSING_MODULE_H_ */
