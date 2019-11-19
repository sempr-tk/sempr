#ifndef SEMPR_ENTITY_COMPONENT_HPP_
#define SEMPR_ENTITY_COMPONENT_HPP_

#include <memory>

namespace sempr {
namespace entity {

/**
    Components are the data-blobs that make up an entity.

TODO: Inherit rete::WME, or create a rete::WME that wraps Component?
*/
class Component {
public:
    Component();
    virtual ~Component();
    using Ptr = std::shared_ptr<Component>();

    /**
        Triggers an UPDATE signal in order to re-evaluate rules that depend on
        the component.
    */
    void changed();
};

};
};

#endif /* end of include guard: SEMPR_ENTITY_COMPONENT_HPP_ */


