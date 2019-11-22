#ifndef SEMPR_ENTITY_COMPONENT_HPP_
#define SEMPR_ENTITY_COMPONENT_HPP_

#include <memory>

namespace sempr {
namespace entity {

    class Entity;

/**
    Components are the data-blobs that make up an entity.

TODO: Inherit rete::WME, or create a rete::WME that wraps Component?
*/
class Component {
    /// a pointer to the entity that this component belongs to.
    Entity* entity_;

    friend class Entity;
public:
    Component();
    virtual ~Component();
    using Ptr = std::shared_ptr<Component>;

    /**
        Triggers an UPDATE signal in order to re-evaluate rules that depend on
        the component.
    */
    void changed();
};

};
};


// also add a specialization for rete::util::to_string for Component to be able
// to use it in the TupleWME.
// TODO: Maybe I should implement a real custom WME which inherits TupleWME?

#include <rete-core/Util.hpp>

namespace rete { namespace util {

template <> std::string to_string(const sempr::entity::Component& c);
template <> std::string to_string(const sempr::entity::Component::Ptr& c);

}}

#endif /* end of include guard: SEMPR_ENTITY_COMPONENT_HPP_ */


