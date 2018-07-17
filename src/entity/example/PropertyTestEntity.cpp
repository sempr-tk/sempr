#include <sempr/entity/example/PropertyTestEntity.hpp>
#include <PropertyTestEntity_odb.h>


namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(PropertyTestEntity);


PropertyTestEntity::PropertyTestEntity()
    : SemanticEntity(new core::IDGen<PropertyTestEntity>())
{

    // register member variables

    this->registerProperty("<http://foo.bar/someInt>", intValue_);
    this->registerProperty("<http://foo.bar/someFloat>", floatValue_);
    this->registerProperty("<http://foo.bar/someDouble>", doubleValue_);
    this->registerProperty("<http://foo.bar/someString>", stringValue_);
    this->registerProperty<Person::Ptr>("<http://foo.bar/someEntity>", entityValue_);

    // register multiple times! but now as plain_string
    this->registerPropertyPlain("<http://foo.bar/somePlainInt>", intValue_);
    this->registerPropertyPlain("<http://foo.bar/somePlainFloat>", floatValue_);
    this->registerPropertyPlain("<http://foo.bar/somePlainDouble>", doubleValue_);
    this->registerPropertyPlain("<http://foo.bar/somePlainString>", stringValue_);
    this->registerPropertyPlain("<http://foo.bar/somePlainEntity>", entityValue_);


    // TODO composite value
}

} /* entity */

} /* sempr */
