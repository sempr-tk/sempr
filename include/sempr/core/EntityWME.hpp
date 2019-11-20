#ifndef SEMPR_CORE_ENTITYWME_HPP_
#define SEMPR_CORE_ENTITYWME_HPP_

#include <rete-core/ReteCore.hpp>
#include "sempr/entity/Entity.hpp"
#include "sempr/core/Utility.hpp"

namespace sempr {
namespace core {

/**
    A wrapper class to allow insertion of arbitrary entities into the reasoner.
*/
class EntityWME : public rete::WME {
    static const std::string type_;
    std::shared_ptr<sempr::entity::Entity> entity_;
public:
    using Ptr = std::shared_ptr<EntityWME>;
    /**
        Construct a wrapper WME around the given entity
    */
    EntityWME(sempr::entity::Entity::Ptr);

    sempr::entity::Entity::Ptr entity() const;
    std::string toString() const override;

    const std::string& type() const override;

    /**
        Comparison of entities is simple: As they are first class objects and not just
        (possibly redundant) data, every entity is seen as unique. We can thus compare by id alone.
        (Other WMEs might want to do this differently, e.g. TupleWME and Triple)
    */
    bool operator < (const rete::WME& other) const override;
};

} /* core */
} /* sempr */


#endif /* end of include guard: SEMPR_CORE_ENTITYWME_HPP_ */
