#ifndef SEMPR_PROCESSING_DEBUGMODULE_H_
#define SEMPR_PROCESSING_DEBUGMODULE_H_

#include <Entity_odb.h>

#include <sempr/processing/Module.hpp>
#include <sempr/entity/Entity.hpp>

namespace sempr { namespace processing {

class DebugModule : public Module<core::Event, core::EntityEvent<entity::Entity>> {
public:
    using Ptr = std::shared_ptr<DebugModule>;
    DebugModule();
    virtual ~DebugModule();

    virtual std::string type() const override;

    void process(core::Event::Ptr e) override;
    void process(core::EntityEvent<entity::Entity>::Ptr e) override;
};

} /* processing */

} /* sempr */

#endif /* end of include guard: SEMPR_PROCESSING_MODULE_H_ */
