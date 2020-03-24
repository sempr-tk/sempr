#ifndef SEMPR_ENTITY_COMPONENT_HPP_
#define SEMPR_ENTITY_COMPONENT_HPP_

#include <memory>

// enable json serialization for all components
#include <cereal/archives/json.hpp>
#include <cereal/types/polymorphic.hpp>

#include "Utility.hpp"

namespace sempr {
    class Entity;

/**
    Components are the data-blobs that make up an entity.
*/
class Component : public std::enable_shared_from_this<Component> {
    /// a pointer to the entity that this component belongs to.
    Entity* entity_;

    /// a string-tag attached to the component. completely optional, but if used
    /// must be set before adding the component to an entity.
    std::string tag_;

    friend class Entity;
protected:
    // read-only access to the entity, in order to access e.g. the entities id.
    const Entity* getEntity() const;
public:
    Component();
    virtual ~Component();
    using Ptr = std::shared_ptr<Component>;


    /**
        Sets the tag for this component.

        \throws sempr::Exception if the component has already been added to an
                entity.
    */
    void setTag(const std::string& tag);

    /**
        Returns the tag assigned to this component.
    */
    std::string getTag() const;

    /**
        Triggers an UPDATE signal in order to re-evaluate rules that depend on
        the component.
    */
    void changed();


    /**
        Serialization with cereal
    */
    template <class Archive>
    void save(Archive& ar) const
    {
        ar( cereal::make_nvp<Archive>("tag", tag_) );
    }

    template <class Archive>
    void load(Archive& ar)
    {
        ar( cereal::make_nvp<Archive>("tag", tag_) );
    }
};

// specialization for sempr::ComponentName<class C>
template <>
struct ComponentName<Component> {
    static constexpr const char* value = "Component";
};

}


CEREAL_REGISTER_TYPE(sempr::Component)


// also add a specialization for rete::util::to_string for Component to be able
// to use it in the TupleWME.
// TODO: Maybe I should implement a real custom WME which inherits TupleWME?

#include <rete-core/Util.hpp>

namespace rete { namespace util {

template <> std::string to_string(const sempr::Component& c);
template <> std::string to_string(const sempr::Component::Ptr& c);

}}

#endif /* end of include guard: SEMPR_ENTITY_COMPONENT_HPP_ */


