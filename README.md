# Semantic Environment Mapping, Processing and Reasoning (SEMPR)
## Status
As the development of SEMPR just started recently, please do not expect a fully featured system. We are working on it. :)

## Installation

SEMPR currently relies on a few other libraries: ODB for the object relational mapping, sqlite3 for a database connection, boost for uuids, soprano for SPARQL queries, qt4 for soprano, and a recent version of gdal with geos support for more geometric operations.

### ODB, GeographicLib, soprano, boost, geos 
For a start, install the odb compiler and the required odb-libraries. If you are on Ubuntu 16.04 you can install them easily:
```bash
sudo apt-get install odb libodb-sqlite-2.4 libodb-boost-2.4 libsqlite3-dev libodb-dev
```
Else please follow the instructions [here](https://www.codesynthesis.com/products/odb/download.xhtml).

Soprano and boost can be retrieved just as easily, **if you are okay with using qt4**:
```bash
sudo apt-get install libsoprano-dev qt4-default
sudo apt-get install libboost-all-dev
```
The boost-part is a bit overkill, but I don't really know which part of boost is needed. You could try `libboost-dev`...

**If you need qt5** instead, you will need a customized build of soprano: SEMPR uses Soprano, and qt to work with it. Whatever version of qt is used by soprano will also be used by SEMPR, as it is propagated through pkg-config. Compiling soprano with qt5-support can be a bit tricky, especially if you are simultaneously using `libcurl4-openssl-dev`, e.g. if you have ROS installed. But don't worry, everything is explained in detail at the wiki page on [how to enable qt5 support](https://github.com/sempr-tk/sempr/wiki/soprano_qt5).

For the spatial geometries you also need GEOS and the GeographicLib to transform from and to global coordinate systems:

```bash
sudo apt-get install libgeos-dev libgeos++-dev
sudo apt-get install libgeographic-dev libeigen3-dev
```

### SEMPR
Now, to actually compile (and install) SEMPR (libsempr-core):
```bash
cd ~
git clone http://github.com/sempr-tk/sempr
mkdir sempr_install
cd ~/sempr
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=~/sempr_install
make -j8
make install
```
Again, to be able to find SEMPR in your other projects, update your `PKG_CONFIG_PATH`:
```bash
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:~/sempr_install/lib/pkgconfig
```
You can now use
```cmake
pkg_check_modules(SEMPR REQUIRED sempr-core)
include_directories(${SEMPR_INCLUDE_DIRS})
link_directories(${SEMPR_LIBRARY_DIRS})
target_link_libraries(foo ${SEMPR_LIBRARIES})
add_definitions(${SEMPR_CFLAGS_OTHER})
```
in your own cmake-project.

> **Note:** *Maybe you noticed the '*add_definitions(...)*' line above. The used database backend is currently fixed at compile-time, and some header files that describe how to serialize the used datatypes for different databases make use of a `-DDATABASE_xxx` flag to select the correct implementation.  This flag is exported in the pkg-config file 'sempr-core.pc'*



### IDE setup

When using this library you may experience very slow autocompletion (at least when using something clang-based with a `.clang_complete`). There are a lot of includes, only few forward declarations, and some template magic (a lot of it introduced by odb to support persistence of entities). To speed things up (***a lot***) you can use the precompiled header file that is automatically created and installed together with the rest of the sempr-core library. Add to your `.clang_complete`:

```
-include-pch
<your-sempr_install-path>/include/sempr/sempr-core.pch
```

> **Note:** *A bug (in clang?) it is required to specify* `-include-pch` *and the actual path in two separate lines or else clang may try to directly include a file called "-pch", and issue an error due to not finding it.*

## License
SEMPR itself is released under a 3-clause BSD license. However, it relies heavily on [ODB](http://www.codesynthesis.com/products/odb/download.xhtml) which is licensed under GPL. Please take a look at the [license exception](LICENSE_ODB) granted by Code Synthesis (copied from [here](https://git.codesynthesis.com/cgit/odb/odb-etc/tree/license-exceptions/sempr-tk-odb-license-exception.txt)) which allows the use of SEMPR (everything belonging to the sempr-tk family) in other projects without imposing additional constraints to the BSD-3-license -- as long as you don't modify the persistent object model for which ODB is used.

## Maintenance and development
DFKI GmbH - Robotics Innovation Center

![dfki-logo](doc/dfki_logo.jpg)


## Usage
``` c++
#include <sempr/sempr.hpp>
using namespace sempr::processing;
using namespace sempr::storage;
using namespace sempr::entity;
using namespace sempr::query;
using namespace sempr::core;

int main(int argc, char** args)
{
	// create a storage module
	ODBStorage::Ptr storage( new ODBStorage() );

	// create the core structure of sempr
	Core core(storage);

	// create processing modules that react on events
	DBUpdateModule::Ptr updater( new DBUpdateModule(storage) );
	ActiveObjectStore::Ptr active( new ActiveObjectStore() );

	core.addModule(updater);
	core.addModule(active);

	// add a new entity
	CoffeeMug::Ptr mug1( new CoffeeMug() );
	core.addEntity(mug1);	// triggers an EntityEvent
	
	// load entities from the database (should be done sparsely).
	std::vector<CoffeeMug::Ptr> mugs;
	storage->loadAll(mugs);
	
	// more often, you'll want to query for an active (previously loaded) object
	ObjectQuery<CoffeeMug>::Ptr mugQuery( new ObjectQuery<CoffeeMug>() );
	core.answer(mugQuery);
	for (auto mug : mugQuery->results)
	{
		std::cout << "mug: " << mug->id() << std::endl;
	}

	return 0;
}
```

## Internal structure
SEMPR consists of a collection of different `Entity`s (things that can be stored in the database) and processing modules. Whenever something changed and the rest of the system needs to be informed, an event can be delivered to the processing modules. Typically, an event stores a pointer to the entity that changed, and the processing modules subscribe to specific types of events (see [processing modules](#processing-modules)).

Events are the way automatic (forward) processing is implemented in SEMPR: Whenever something changed, all processing modules get the chance to react to the change, update their internal datastructure, modify data and trigger more events etc. Since some calculations might be very cost-intensive, methods exist to create information on demand (backward processing/reasoning): Whenever a `Query` is to be answered, not only the currently existing data is checked for an answer, but the query itself is handed to the processing modules in order to extend the results. while processing a query, the modules may create sub-queries of their own. This exhibits a risk of endless loops -- later implementations might introduce clever strategies to cope with this problem, e.g. a depth-limit.

**TODO:** _This has not been implemented yet._

The main focus of processing modules lies on symbolic and spatial reasoning. Hence, interfaces are expected to provide this information - symbolic knowledge and geometries - in an orderly manner. Therefore, currently two subclasses of `Entity` are proposed: `RDFEntity` and `GeometricEntity`. Complex entities that aggregate other entities should do exactly that: E.g., `SomeSpecialEntity` might be represented by a mesh plus semantic information. If for whatever reason you want to inherit from an Entity, be aware: Virtual inheritance is not supported by the underlying database, at it heavily relies on static casts (and one cannot static-cast through a virtual base).
Moreover, there may be different specializations, e.g. `MeshEntity` or `PointEntity`. See the diagram below.

![alt](doc/Entity-Structure.png)

>**Something to think about:**
Assume the following structure (simplified):
``` c++
class Something {};
class Base {};
class Derived : public Base {
public:
 	Something* else_;
};
```
> What does it mean to change any part of this structure? Which events should be triggered?
Should modifying through `Base` also trigger `Derived::changed()`? Sure.
Should modifying through `Derived`also trigger `Base::changed()`? Maybe.
Should modifying `Something` through `Derived`s pointer trigger `Derived::changed()`? Maybe.
Should modifying `Something`through `Derived`s pointer trigger `Base::changed()`? Maybe not?

Care must be taken that every relevant type of event is fired when an entity is created / loaded / changed or erased. When entities are added to the core, they are given access to the event-broker, so that they are able to inform the system about changes.

### Entity Ownership
The question of _entity-ownership_ is a hard one -- who creates (i.e.: persists), updates and removes an entity? In the case of complex entities that consist of multiple other entities -- e.g. a `CoffeeMug` that holds semantic information in an `RDFEntity` and its geometric representation in a `GeometricEntity` -- it would be convenient to let the entity itself manage its sub-entities. There are a few things that have to be taken care of:

- When an entity is persisted or updated in the database, its **pointers must be valid**: The pointed-to entity must have already been persisted in the database, else you will violate a `foreign-key-constraint`.
- The created entity must be **advertised to the system** to allow processing modules to do their job. Therefore, it needs a pointer to the `EventBroker`.

SEMPR provides a mechanism to do this semi-automatically: Whenever an `Entity`creates another `Entity` and keeps a pointer to it, it needs to register it as a child using:
``` c++
void Entity::registerChildEntity(Entity::Ptr)
```
On the first announcement (`loaded()` or `created()`)  of an entity, it also announces all children that are registered so far. To be exact, it announces the children prior to the parent: This assures that the children are persisted first. On removal, the parents removed-event is issued first, for the same reason of avoiding inconsistent/invalid database states.

Whenever you want to register a child entity make sure that either of these conditions holds true:
1. Both parent and child have not been announced to the system yet, so that the parent can correctly handle its children.
2. Both parent and child have already been announced (and thus persisted), so that no special treatment is necessary.

Also, you need to be aware that this mechanism is not fool-proof: No one prevents you from creating circular dependencies here, by registering the parent as a child of child. This _will_ be problematic, as you won't be able to remove one without a dangling reference, and thus a broken foreign-key-constraint.

A typical usage of `registerChildEntity` would be to register entities that are created in the c'tor of another entity, as part of it. This would fulfill condition (1.).


## Storage
Interface to be implemented by any storage-module.
### ODBStorage
First implementation for a database backend.
### DBObject
A base class for objects one wants to persist in the database. Uses a boost-uuid
and maintains a weak_ptr to a parent-object that owns it: If the parent is deleted
from the database, so is this object.
### History
Template class to maintain a history of objects/values: Stores the given type T
in a vector and is implicitly convertible to the T, returning a reference to the
newest entry.
``` c++
History<int> balance;
balance = 0;
balance = 123;
balance = 200;
cout << "size:" << balance.size() << "\n";
cout << "current: " << balance << "\n";
cout << "first:" << balance[0] << "\n" ;
```
Will print:
```
size: 3
current: 200
first: 0
```
**TODO:** _store timestamps and use them to index into the vector_

## Entity
Entities are logical sets of data in SEMPR. You may use the one we've already provided or create new ones for your own special use-case.
There are a few things that need to be remembered when creating a new entity-class:

1. **Declare your class as a database object.** Use `#pragma db object`.

2. ODB needs to be able to access your data members. **Add a `friend class odb::access`**

3. **Set the discriminator.** ODB auto-generates a string to identify derived classes of DBObject, but only sets it as a class-trait and saves it in a special column in your database tables. SEMPR loads the discriminator from the databse automatically, but to have it available for newly created instances, you need to call `setDiscriminator<MyClass>();`in the constructor of `MyClass`.

   > **Todo:** If we ever extend the type-registry from events and queries to entities as well, we could do this in something similar to OType\<T\>.

4. **Enable specific ID generation.** If you dont want all your objects to be called `DBObject_%i`, you will want to pass on a customized ID-generator. The DBObject-class accepts an instance of `IDGenBase`, so you may give it an `IDGen<MyClass>`-object. In turn, you may want to accept a generator yourself to allow derived classes to specify their own specific IDs. However, the following attempt will lead to some problems:

   ```c++
   /* MyClass.hpp */
   MyClass(const core::IDGenBase& gen = core::IDGen<MyClass>())  // error!
   	: Entity(gen)
   {
    	// ctor stuff
   }
   ```

   Since some strategies may want to use the type-traits defined by odb we cannot use `core::IDGen<MyClass>` in the header file of `MyClass` -- odb must compile the header first and create a `MyClass_odb.h` which can be included in `MyClass.cpp` and provides the type traits we need. Therefore, the following approach is to be used (also, since the DBObject-class needs access to the generation-object later on to free its own id on removal, we pass it a pointer. DBObject takes care of deletion):

   ```c++
   /* MyClass.hpp */
   class MyClass : public Entity {
   	/// default ctor will use IDGen<MyClass>
   	MyClass();
   
   	/// alternative to allow derived classes to pass their own IDGen<DerivedClass>
   	MyClass(const core::IDGenBase* gen);
   };
   
   /* MyClass.cpp */
   MyClass::MyClass()
   	: MyClass(new core::IDGen<MyClass>())
   {
   	// empty. ctor delegation
   }
   
   MyClass::MyClass(const core::IDGenBase* gen)
   	: Entity(gen)
   {
   	// ctor stuff
   }
   ```

   The DBObject takes care of freeing IDs, so that they can be reused later on. This is only relevant for the `IncrementalIDGeneration`: If you create Object\_1 to 10 and remove Object\_3, "Object\_3" joins the pool of revoked ids and can be assigned to the next object to be created.

5. **Use the `SEMPR_ENTITY` macro.** It declares a method that overrides `Entity::createEntityEvent` and is used to generate correct events for your entities. If you miss adding this, your `Polygon`-Entity derived from `Geometry` will only emit `EntityEvent<Geometry>` when its created, loaded, changed or removed. Due to a dependency on the odb-generated type-traits, the actual implementation can only happen in the source file by adding **`SEMPR_ENTITY_SOURCE(classname)`**. It does not matter where you put these macros -- the private scope of the class is just fine:

   ```c++
   /* MyClass.hpp */
   class MyClass : public Entity {
       SEMPR_ENTITY
       // [...]
   };
   
   /* MyClass.cpp */
   SEMPR_ENTITY_SOURCE(Polygon)
   ```


So, all in all the basic structure for your custom entity looks like this:

```c++
/* MyClass.hpp */
#include <sempr/entity/Entity.hpp>

#pragma db object
class MyClass : public Entity {
    SEMPR_ENTITY;
    friend class odb::access; // if you have private/protected members
public:
	/// default ctor will use IDGen<MyClass>
	MyClass();

	/// alternative to allow derived classes to pass their own IDGen<DerivedClass>
	MyClass(const core::IDGenBase* gen);
};



/* MyClass.cpp */
#include <MyClass.hpp>
#include <MyClass_odb.h> // generated by odb. Actually includes MyClass.hpp

SEMPR_ENTITY_SOURCE(MyClass)

MyClass::MyClass()
	: MyClass(new core::IDGen<MyClass>())
{
	// empty. ctor delegation
}

MyClass::MyClass(const core::IDGenBase* gen)
	: Entity(gen)
{
	// ctor stuff
    this->setDiscriminator<MyClass>();
}
```



### RDFPropertyMap
The RDFPropertyMap is a utility entity that allows its user to store values of different types in a single datastructure. Internally, it uses `Soprano::LiteralValue`s (which are based on `QVariant`) to store the most common datatypes. As the name implies, it is a map-structure: String-keys are mapped to values, and since it is derived from `RDFEntity`, every key-value-pair can be seen as a triple. Therefore, the map has to be given a subject to use in the triples as well as a base-URI that precedes the keys to form valid properties in RDF. E.g.:
```c++
RDFPropertyMap::Ptr map(new RDFPropertyMap("subject", "http://baseURI/"));
core.addEntity(map);

// syntactic sugar to avoid having to write (*map)["key"]
RDFPropertyMap& m = *map;
m["foo"] = 42;
m["bar"] = 1.234f;
m["baz"] = "Hello, World!";

// create another entity and point to it.
Person::Ptr person(new Person());
core.addEntity(person);

m["uncle"] = person;
```
This snippet maps to the following set of triples:
```
(subject <http://baseURI/foo> "42"^^xsd:integer)
(subject <http://baseURI/bar> "1.234"^^xsd:float)
(subject <http://baseURI/baz> "Hello, World!"^^xsd:string)
(subject <http://baseURI/uncle> <http://baseURI/[persons-uuid]>)
```
By using an `RDFPropertyMap` other entites are able to store their information and provide them in an RDFEntity without much redundancy and overhead. Whether you want to use the `RDFPropertyMap` exclusively or just use it to store otherwise transient data members (during prePeresist/preUpdate/postLoad) is up to you -- as long as the map gets the values they are available to (optional) reasoners.

#### Problems / Usage
The RDFPropertyMap is able to store literal values as well as pointers to other objects which will be represented as rdf resources. One problem arises when trying to store resources that are not pointers: How can we distinguish between the string `<http://example.com/foo>` and the resource uri (which looks the same)? The solution chosen here is the following: The RDFPropertyMap assumes that objects are to be interpreted as literal values, except when told otherwise. To prevent the resource string to be interpreted as a literal string (and thus stored and given to the reasoners as `"<http://example.com/foo>"^^<http://www.w3.org/2001/XMLSchema#string>`) you need to specify that it is indeed a resource:
```c++
m["foo"] = RDFResource( "<http://example.com/foo>" );
```

Furthermore you might want to save triples with a property that does not belong to the namespace that you specified in the `baseURI` of the property map. This might be the case if you want to add e.g. just a note on the type of object, i.e. a triple like `(sempr:Person_42 rdf:type sempr:Person)`. This can be achieved by explicitely specifying the baseURI of the property you want to access:
```c++
m("type", rdf::baseURI()) = RDFResource( "<" + rdf::baseURI() + "Person>" );
```
> Please note the round brackets as c++ does not allow multiple arguments for the square-bracket-operator. For the single-argument version you may use any type: `m["foo"]` and `m("foo")` are interchangable.

There are more things to consider when working with an RDFPropertyMap: Internally, everything is stored in instances of RDFValue. These provide the interface to assign them values of (more or less) arbitrary type, and to cast them to the type you previously stored in it (and still know what it is!). The RDFValue class _tries_ to be as transparent as possible to the user by providing implicit cast-methods, and even implements some operators (`<, <=, ==, !=, >=, >`) to allow comparison of an RDFValue with a literal value, by casting the RDFValue to the right-handed side of the operation and using the operator of the result:

```c++
if (m["foo"] > 12) do_something(); // m["foo"].get<int>() > 12
```

Sometimes the casts can be a bit tricky, and ambiguous. When something goes wrong you can still try to use the `template <typename T> get()` method directly to cast the RDFValue to the desired type. If you want to compare strings e.g., the previously mentioned operators are overloaded to cast the RDFValue to std::string when comparing with a const char*:

```c++
if (m["baz"] == "Hello, World!") do_something(); // m["baz"].get<std::string> == "Hello, World!";
```

The `get<T>()` method is just another way to explicitly call the `operator T()`, and its implementation is short enough to fit into this README :wink: :

```c++
template <typename T>
T get() {
    return *this; // uses "template <typename T> operator T()" to cast to T.
}
```



### SemanticEntity

An alternative to the RDFPropertyMap is the SemanticEntity-class. Both achieve the same goal: They provide an RDF-Triple representation of its given values. But the actual implementation and usages differ quite a lot: The RDFPropertyMap stores the given values itself and associates them with string-keys. This means that you are free to add values as you please, from any source, but you need to know exactly the key and type of the value whenever you want to access it -- the compiler won't help you there, you lose static type safety. The SemanticEntity implements the other extreme: It does not store the values itself but keeps only references to variables, and thus has some restrictions on what can be stored. But since your data stays in your usual c++ member variables, you can work with them without any conversions.

####Implementation details

The SemanticEntity class provides the methods:

```c++
template<class T> void registerPropetry(const std::string& predicate, T& property);
template<class T> void registerProperty(const std::string& subject, 
                                        const std::string& predicate, T& property);

template<class T> void registerPropertyPlain(const std::string& predicate, T& property);
template<class T> void registerPropertyPlain(const std::string& subject, 
                                             const std::string& predicate, T& property);
```

Which can be used to register a variable and thus make it available as a triple. The value of the variable will always be presented in the "object" part of the triple. The "predicate" part of the triple needs to be specified, whereas the "subject" part is optional: If omitted, the ID of the SemanticEntity is used, and the subject will be `"<" + sempr::baseURI() + entity->id() + ">"`, e.g. `<sempr:SemanticEntity_42>`.

Since the variable is given by reference you must ensure that its lifetime exceeds/is coupled to the lifetime of the SemanticEntity. You should only register member variables of the SemanticEntity itself, and thus have to _derive_ from SemanticEntity, which is why the above methods were made _protected_. 

> **TODO**: _In the future, methods to check the lifetime of the owner of the variable may be added in order to use the SemanticEntity in a compositional fashion, just like the RDFPropertyMap, without inheriting it, but those are not implemented yet._

Internally, for every registered property an instance of `RegisteredPropertyBase` is stored in a vector of the SemanticEntity. Those objects simply provide an interface to `virtual std::string object() const = 0;` which is provided by the (templated) derived classes `RegisteredProperty`. The `RegisteredProperty`s in turn have to major modifications:

1. They also provide a `bool isValid() const;` which is implemented differently for `std::shared_ptr`, in which case they return if the property/variable/member is not null. In case of non-shared-ptrs it always returns true. This method is used to check if a registered property should be skipped on iteration, as a nullptr is (IMHO) best represented by the absence of a triple.
2. The RegisteredProperty-template takes as a second template argument another template referenced to as `ToString`. This can be used to switch between different modes: Currently implemented are the templates `sempr::rdf::traits::n3_string` and `sempr::rdf::traits::plain_string` which are used by the methods `SemanticEntity::registerProperty` and `SemanticEntity::registerPropertyPlain` respectively. The former converts the given value to N3 notation, e.g. `"42"^^<http://www.w3.org/2001/XMLSchema#int>`, whereas the latter converts the values to plain strings, e.g. `42`. 

The currently supported datatypes may be a bit limited: The default definition for `n3_string` uses sopranos literal nodes for conversion, the default for `plain_string` utilizes `std::to_string`. Both have a specialization for shared pointers to types derived from `sempr::entity::Entity`, in which case the id of the entity is used to construct the rdf value. If you want to extend the capabilities of the SemanticEntity to your own types than this is your starting point: Implement your own conversion functions.

#### Usage

If you want to use the approach of the SemanticEntity to make certain member variables of your class visible as an RDF triple, do the following:

1. Implement your class as described in the previous sections, but derive from SemanticEntity.
2. In your constructor, use `registerProperty` or `registerPropertyPlain` to make a member variable visible.

For example (parts from the PropertyTestEntity):

```c++
// PropertyTestEntity.hpp

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
};
    
}}
```

```c++
// PropertyTestEntity.cpp
#include <PropertyTestEntity_odb.h>


namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(PropertyTestEntity);


PropertyTestEntity::PropertyTestEntity()
    : SemanticEntity(new core::IDGen<PropertyTestEntity>())
{
    this->setDiscriminator<PropertyTestEntity>();

    // register member variables   
    this->registerProperty("<http://foo.bar/someInt>", intValue_);
    this->registerProperty("<http://foo.bar/someFloat>", floatValue_);
    this->registerProperty("<http://foo.bar/someDouble>", doubleValue_);
    this->registerProperty("<http://foo.bar/someString>", stringValue_);
    this->registerProperty("<http://foo.bar/someEntity>", entityValue_);
}

}}
```

It's as easy as that. Since a SemanticEntity implements the RDFEntity-Interface, whenever you call `PropertyTestEntity::changed()` all processing modules listening for RDFEntitys are informed, too, and when iterating over the triples of your entity have access to the current values of the registered properties.

### Geometry
To store geometric data, SEMPR relies on [GDAL](http://www.gdal.org/), which implements a standard proposed by the _Open Geospatial Consortium (OGC)_. The class hierarchy of `OGRGeometry` is mirrored in entities with the `Geometry`-base-class. The entity-classes simply wrap a corresponding geometry: `entity::Point` contains an `OGRPoint*`etc.

#### Spatial reference systems / coordinate systems
Geometric data is not always stored in the same global reference system. In some cases it might be even beneficial to create local coordinate systems that are related to each other: If a parent system is changed, the children stay in relative position/orientation to their parent (and change on a more global scale). In order to support such chains and different reference systems -- even geographic ones -- the following distinctions have been made, and classes have been implemented.
##### Global references
Global reference systems inherit from the `GlobalCS`-class and are implicitly linked to each other, with the earth as a common root. Since "earth" is not modeled explicitely, the global references are also referred to as "root reference systems". Currently, they are supported through two classes: `GeographicCS` models geographic coordinate systems which are based on latitude and longitude coordinates. They do cannot be chained with other reference systems: How would an affine transformation relative to "WGS84" make any sense?

In most cases you will want to represent data that is not directly aquired in (lon lat) but rather (x y z) coordinates, but which may be linked to a specific geographic location. This is what a `ProjectionCS` is used for: It defines a reference system projected to/from an underlying geographic one. Currently implemented are UTM-frames (*note: pure UTM,* **not** *MGRS!*) and equirectangular projections. The latter can be attached to any (lat lon) coordinate and defines a plane that touches the globe at that location. Since at this point the coordinates of any geometry attached to the coordinate system is interpreted as (x y z), it is possible to attach local reference systems to it.
##### Local references
Local reference systems simply define affine transformations (currently only rotation and translation) between each other, every `LocalCS` having at most one parent coordinate system, which can be another `LocalCS` or a `ProjectionCS`.

##### Geometry transformation
The `Geometry`-class implements methods to set the current reference systems as well as transforming it into another one. There are a few things that can go wrong during `Geometry::transformToCS`, so it may throw an exception on you. If it does, the geometry remains in a valid, unchanged state.

There exists some computational overhead for the transformations: First of all, the transformation is computed whenever it is needed, which means that the tree of reference frames is traversed from the source and target systems to their respective roots -- which shouldn't be too expensive. After that, there are two possibilities:

1. The root coordinate systems are the same. In this case, one full transformation can be computed (source --> root --> target) and applied to the geometry in one iteration over its coordinates.
2. The root coordinate systems are different. Since GDAL takes care of the transformation between them, and the local systems are an addition of sempr, we need three steps to transform the geometry: 
	1. Transform from source local to source root
	2. Let GDAL transform it from source root to target root
	3. Transform from target root to target


#### Serialization
The `traits-sqlite-geometry.hxx` implements a traits-class for `OGRGeometry*` with templated methods to store any pointer to an `OGRGeometry` or derived class. Currently, only a selected subset of GDALs geometries is supported, but serialization support is easily extended by inheriting from this traits-class: E.g., the line:

```c++
template <> class value_traits<OGRPolygon*, id_blob> : public value_traits<OGRGeometry*, id_blob> {};
```

enables serialization of `OGRPolygon*` in binary form (WKB). In your entities, use e.g.:
```c++
#pragma db type("BLOB")
OGRPolygon* polygon_;
```

## Events & Queries

As stated before, communication between `ProcessingModule `s in SEMPR is based on events and queries. In some cases, inheritance is a natural way to describe them: A `Geometry` is an `Entity`, and `Point`, `LineString`, `Polygon` etc. are all derived from `Geometry`. What happens when a polygon in sempr changed? We expect an `EntityEvent<Polygon>`, and we *also* expect a processing module that processes only `EntityEvent<Geometry>` to handle this one, too. On the query-side of things we have a similar situation in which we want to query on a C++ level for certain object types, and therefore have e.g. `ObjectQuery<Point>` and `ObjectQuery<Polygon>` which are both derived from a common `ObjectQueryBase`, which can be processed by our `ActiveObjectStore` (a processing module). Together with our wish be able to easily add new entities, events and queries, this leads us to a problem:

### Multiple-dispatch-problem

In the core of sempr we have two variables of "unknown" type: The processing modules used as well as the events and queries that are to be processed are both only known as their respective base types. So let's consider the following (simplified) example:

```c++
Module* m = new VerySpecialProcessingModule();
Event* e = new SomeStrangeEvent();

m->process(e);
```

Well, if the `VerySpecialProcessingModule` has an overloaded method `process(SomeStrangeEvent*);` it should handle everything correctly, right? **Wrong.** In C++, only single-dynamic-dispatching is possible, namely through the use of virtual methods. But the type of the argument is `Event*`, so the method `VerySpecialProcessingModule::process(Event*);` will be called. We can circumvent this problem by using the visitor-pattern -- but this leads us to very static, not well extensible code. Another way would be to use `dynamic_cast` (or rather `std::dynamic_pointer_cast`), but we've chosen a different approach: Events and queries share a common base class called `Observable` which provides the following methods:

```c++
template <class T> void registerType();
template <class T> bool isA() const;
```

These are used to save the `std::type_index` of `T` in a set and check (at runtime) if a given type is registered in the observable before using a `static_cast`. But we still need to make use of these and distribute the observables to the correct methods of a processing module. Therefore, the processing modules are subclasses of `Observer` which allows us to store pairs of check- and process-functions that operate on observables. Whenever `Observer::doProcess(Observable::Ptr)` is called by the core, all check-functions are used to determine which corresponding process-methods to call. But all this is abstracted away by the `Module`-class you will be using when implementing your own [processing module](#processing-modules).

When implementing your own events and queries, you can use the helper class `OType<class T>` to avoid having to call `registerType` in your constructors:

```c++
class MySpecialQuery : public Query, public OType<MySpecialQuery> {};
```



### EntityEvent

Though you may specify and use your own events, the most used events are notifications on when an entity is created, loaded, changed or removed. These are all encapsulated in the class `EntityEvent<T>`, where `T` is the type of entity it is referring to. Since these events are so common and needed for all entities we ever care to define, its usage has been simplified through the use of some template magic: The ODB compiler already defines some traits for us, from which `odb::object_traits<T>::base_class` is especially interesting for us. Using this typedef, the `EntityEvent` class automatically constructs an inheritance chain in parallel to the inheritance chain of the entity type `T`. That means that `EntityEvent<Polygon>` automatically inherits `EntityEvent<Geometry>` (and so on...), registering the types and thus making them usable for the processing modules.

But in order for this to work the `odb::object_traits` **must** have been defined. If `EntityEvent<T>` is instantiated for a `T` that is a subclass of `Entity`, but has no `odb::object_traits<T>::base_class` defined, a compiler error is generated that informs you that you've forgotten to `#include <[...]_odb.h>`.

Moreover, all `object_traits` must be defined *before* the first instantiation, so you will need to include the odb-generated files first.

### EventBroker

## Processing Modules
SEMPR can be extended by adding processing modules that react to events and accept queries as you need them. To create a custom processing module you can inherit from `ModuleBase` and register your own check- and process-methods -- but **it is recommended to use the abstraction** provided by `template<class... ObservableTypes> Module`:

For every type specified in the template argument list of `Module` another method is declared that takes a shared pointer to the type as an input:

```c++
virtual void process(std::shared_ptr<ObservableType>) = 0;
```

This method is automatically called whenever an instance of `ObservableType` **or a subclass of it** is handed to the module. In order to create a processing module that, e.g., reacts to changes in `RDFEntity`s and also answers `SPARQLQuery`s, you would start with the following:

``` c++
class CustomModule 
    : public Module< EntityEvent<RDFEntity>,
                     SPARQLQuery >
{
protected:
    void process(EntityEvent<RDFEntity>::Ptr event) override;
    void process(SPARQLQuery::Ptr query) override;
};
```

If you forget to implement a process-method you will be greeted by a compiler error, as well as when you implement it but forgot to specify the type in Modules argument list. (Note: This relies on the `override` keyword, so make sure to add it. :slightly_smiling_face:)

### Query

Queries are in many ways treated equal to events, except that events are not expected to return a result. While sempr is still single-threaded and every query and event is handled one by one, the difference between queries and events is more or less a naming convention.

Since we cannot predict all kinds of query languages and return values, queries are simple objects that must contain their results. When a module is asked to answer a query it should append the generated results to the queries result list, which can be expanded by following modules. After a call to `Core::answerQuery` all modules will have had the chance to contribute to the answer.

#### ObjectQuery

One basic functionality is to query for objects of a given type that fulfill certain criteria. This is implemented in the templated query `ObjectQuery<T>`. With `T` being an entity-type it can be given a function to decide whether an entity should be returned in the result or not. Basically, the `ActiveObjectStore` iterates over all known entities and calls this decision-function on them, including those entities in the result for which the function returned `true`. 

> **Note**: The type-registry mechanism is only implemented for events and queries, yet. It is not available for the entities themselves, so the ObjectQuery will only return entities of the exact given type: ObjectQuery\<Geometry\> will *not* include points or polygons.

Example: Return all "Person"s from the list of currently active (i.e. already loaded or newly created) entities:
```c++
// setup the core and database
ODBStorage::Ptr storage( new ODBStorage() );
Core core(storage);

// the ActiveObjectStore is a module that answers "ObjectQuery"s
ActiveObjectStore::Ptr active(new ActiveObjectStore());
core.addModule(active);

// create a query, let the core process it and print the results.
auto q = std::make_shared<ObjectQuery<Person> >();
core.answerQuery(q);
for (auto p : q->results) {
    std::cout << "Person: " << p->id() << std::endl;
}
```

And to return only those called "John Smith", pass a lambda expression to the query constructor:

```c++
auto q2 = std::make_shared<ObjectQuery<Person>>(
    [](Person::Ptr person) {
        return person->name() == "John Smith";
    }
);
core.answerQuery(q2);
```



#### SPARQLQuery

The `SopranoModule` keeps track of rdf triples and responds to SPARQL-queries. The `SPARQLQuery`-class uses a default list of prefixes to resolve `rdf`, `rdfs`, `owl`, `xsd` and (of course) `sempr` to their corresponding URIs.

Excerpt from the test cases:
```c++
// [...]
// on initialization, add the SopranoModule
SopranoModule::Ptr semantic(new SopranoModule());
core.addModule(semantic);

// [...]

// insert a few persons
{
    for (int i = 10; i < 20; i++) {
        Person::Ptr p(new Person());
        p->age(i);
        p->height(1.5 + 0.01*i);
        core.addEntity(p);
    }
}

// query for persons of age 18 or older and get their heights
{
    SPARQLQuery::Ptr query(new SPARQLQuery());
    query->query = 
    	"SELECT * WHERE { " \
            "?p rdf:type sempr:Person." \
            "?p sempr:age ?age ." \
             "FILTER(?age >= 18) ." \
            "?p sempr:height ?height ." \
         "}";
     core.answerQuery(query);

     // there should be 2 results, aged 18 and 19
     for (auto r : query->results) {
        // std::cout << "Query Result: " << r["p"] << " of age " << r["age"] << " is " << r["height"] << " m high." << '\n';
        for (auto b : r) {
            std::cout << b.first << "=" << b.second << "  |  ";
        }
        std::cout << std::endl;
    }
}
```

#### SpatialIndexQuery
The `SpatialIndex` is a processing module which listens for changes in geometries and their coordiante systems, calculates their bounding boxes in the root coordinate system and uses them to maintain a spatial index in form of a boost RTree. 
> **Note:** The SpatialIndex currently assumes that all geometries are referenced in a common root cordinate system and does not check if this assumption holds. We should extend it to work only on a concrete `SpatialReference` and it's children and disregard all other geometries. 

This module answers `SpatialIndexQuery`s: You may specify a geometry in any coordinate system, from which the bounding box is created and passed to the processing module. It returns all geometries with bounding boxes which are fully within or intersect (depending on the query type) the box given in the query.
> **Note:** There are multple points of approximation. For every geometry, the one given to the query as well as everything indexed in the module, its AABB is calculated first inside the geometries reference frame. It is then transformed into the root coordinate system, but may now be no longer axis aligned -- so again the AABB (within root) is calculated from the now oriented BB. All checks (intersects, within) are done with these AABBs within the root system.

To use it you can specify a bounding box and reference frame directly:
```c++
SpatialIndex::Ptr index(new SpatialIndex());
core.addModule(index);

// [...] add / load geometries to fill the index

// create a query from a bounding box
Eigen::Vector3d lower, upper;	// TODO: set values
LocalCS::Ptr cs;				// TODO: specify coordinate system
auto query1 = SpatialIndexQuery::withinBox(lower, upper, cs);

// get results
core.answerQuery(query1);
for (auto r : query1->results)
{
	std::cout << r->id() << std::endl;
}
```
Or use an existing geometry:
```c++
// create a query from a geometry
Geometry::Ptr geo; 	// TODO: specify geometry
auto query2 = SpatialIndexQuery::withinBoxOf(geo);
```
You can also invert the mode or set it directly:
```c++
// invert the mode. "within" becomes "not within" and vice versa
query2->invert();

// explicitely set the mode
query2->mode(SpatialIndexQuery::INTERSECTS);
```

## Pitfalls
### Entity creation
The base `Entity` inherits from `std::enable_shared_from_this<Entity>` to enable entities to create smart pointers to themselves, which is needed e.g. in the `Entity::changed()` method, which creates a `EntityEvent` pointing to itself and fires it through the `EventBroker` that it has been given from the `Core`. **Care** must be taken that `shared_from_this()` is only called after a smart-pointer to the object has already been created! Which in turn means that you should not create entities on the stack, but always into smart pointers:

``` c++
Entity::Ptr e1( new Entity() );					// 1.
// or:
Entity::Ptr e2 = std::make_shared<Entity>();	// 2.
```
But: Version `1.` needs two heap allocations, version `2.` only one. But, as the memory for the entity and the control-structure of the shared pointer are allocated in one call, it cannot be freed until all shared **and** weak-ptrs are gone! Whereas in `1.`, when all shared_ptrs are out of scope, at least the memory of the entity is freed.


## TODO
- Load subset from database
  - Be aware of the session caching everything that's loaded!
- Add entities
  - semantic object
  - geometric object
  - general object (to be wrapped by use-case-classes, plus factory, to avoid the odb compiler)
- Add processing modules
  - something tf-like (use envire)
  - GeometryCache
