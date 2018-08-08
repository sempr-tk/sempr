#ifndef SEMPR_ENTITY_EXAMPLE_PROPERTYTESTENTITY_HPP_
#define SEMPR_ENTITY_EXAMPLE_PROPERTYTESTENTITY_HPP_

#include <sempr/entity/SemanticEntity.hpp>
#include <sempr/entity/example/Person.hpp>

namespace sempr { namespace entity {

#pragma db object
class PropertyTestEntity : public SemanticEntity {
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<PropertyTestEntity>;
    PropertyTestEntity();

    int intValue_;
    float floatValue_;
    double doubleValue_;
    std::string stringValue_;
    Person::Ptr entityValue_;
};

} /* entity */
} /* sempr */

#endif /* end of include guard: SEMPR_ENTITY_EXAMPLE_PROPERTYTESTENTITY_HPP_ */
