#ifndef SEMPR_ENTITY_EXAMPLE_PROPERTYTESTENTITY_HPP_
#define SEMPR_ENTITY_EXAMPLE_PROPERTYTESTENTITY_HPP_

#include <sempr/entity/SemanticEntity.hpp>
#include <sempr/entity/Person.hpp>

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

    // RDFProperty<int> intValue_;
    // RDFProperty<float> floatValue_;
    // RDFProperty<double> doubleValue_;
    // RDFProperty<std::string> stringValue_;
    // RDFProperty<Person::Ptr> entityValue_;

    // TODO test composite value
};

} /* entity */
} /* sempr */

#endif /* end of include guard: SEMPR_ENTITY_EXAMPLE_PROPERTYTESTENTITY_HPP_ */
