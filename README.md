# Semantic Mapping, Processing and Reasoning (SEMPR)
## Status
As the development of SEMPR just started recently, please do not expect a fully featured system. We are working on it. :)

## Installation

SEMPR currently relies on a few other libraries: ODB for the object relational mapping, sqlite3 for a database connection, boost for uuids, soprano for SPARQL queries, qt4 for soprano, and a recent version of gdal with geos support for more geometric operations.

### ODB, soprano, boost
For a start, install the odb compiler and the required odb-libraries. If you are on Ubuntu 16.04 you can install them easily:
```bash
sudo apt-get install odb libodb-sqlite-2.4 libodb-boost-2.4 libsqlite3-dev libodb-dev
```
Else please follow the instructions [here](https://www.codesynthesis.com/products/odb/download.xhtml).

Soprano and boost can be retrieved just as easily:
```bash
sudo apt-get install libsoprano-dev
sudo apt-get install libboost-all-dev
```
The boost-part is a bit overkill, but I don't really know which part of boost is needed. You could try `libboost-dev`...

### GDAL with GEOS
Since we need a recent version of GDAL with GEOS support you will need to compile it from source.  But first, make sure you have GEOS installed as well as proj4 (for coordinate system transformations):

```bash
sudo apt-get install libgeos-dev
sudo apt-get install libproj-dev
```

Now, to install GDAL (feel free to adjust the paths to your liking):

```bash
cd ~
git clone https://github.com/OSGeo/gdal.git
cd ~/gdal
git checkout release/2.3
mkdir install
cd ~/gdal/gdal
```
Compilation may take a while, feel free to grab a cup of tea in the mean time:
```bash
./configure --with-geos=yes --prefix=$HOME/gdal/install
make -j8
make install
```
To finish the installation make sure that the gdal includes and libraries can actually be *found*. Therefore, adjust your `PKG_CONFIG_PATH` (e.g. in your `.bashrc`):
```bash
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:~/gdal/install/lib/pkgconfig
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
Upon persisting or updating the parent entity, the following steps are executed:

1. Before persisting/updating the parent, all newly registered children
	1. get a pointer to the event-broker
	2. get persisted in the database (which might trigger this mechanism again, recursively)
2. After persisting/updating the parent, all newly registered children
	1. are given a pointer to the parent with `on delete cascade` enabled
	2. are updated (to apply the parent-pointer)
	3. are announced to the system through invocation of `child->created()`

The `on-delete-cascade` mechanic is database-internal: Whenever an entity has a valid parent-pointer and the parent is removed from the database, the child-entity is removed, too. This ensures that no orphans remain in the database, but it does not trigger any events on the code-side of things. Hence we need to trigger those events ourselves, which forces us to keep a list of all children. This list is managed inside `Entity`. Whenever the `Entiy::created()`, `Entiy::loaded()` or `Entiy::removed()` is called, the Entity also calls the same method of all of its children, thus firing the respective events for all of them. The parents `created() / removed()` are called when adding/removing it to/from the system (`Core::[add/remove]Entity`).

**TODO: loaded() needs to be called on postLoad(), doesn't it? [Alternative: Storage calls it on loading.... inconsistent...]  To make everything a bit more consistent, do we need to call removed() on postDelete()? but created() has to be separated from the db-callbacks since we might want to create temporary, non-persistent objects...? then again, same goes for removed().**


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

1. ODB needs to be able to access your data members. Add a `friend clas odb::access`
2. Set the discriminator. ODB auto-generates a string to identify derived classes of DBObject, but only sets it as a class-trait and saves it in a special column in your database tables. SEMPR loads the discriminator from the databse automatically, but to have it available for newly created instances, you need to call `setDiscriminator<MyClass>();`in the constructor of `MyClass`.
3. Enable specific ID generation. If you dont want all your objects to be called `DBObject_%i`, you will want to pass on a customized ID-generator. The DBObject-class accepts an instance of `IDGenBase`, so you may give it an `IDGen<MyClass>`-object. In turn, you may want to accept a generator yourself to allow derived classes to specify their own specific IDs. However, the following attempt will lead to some problems:
```c++
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
	: Entity(new core::IDGen<MyClass>())
{
	// ctor stuff
}

MyClass::MyClass(const core::IDGenBase* gen)
	: Entity(gen)
{
	// ctor stuff
}
```

The DBObject takes care of freeing IDs, so that they can be reused later on. This is only relevant for the `IncrementalIDGeneration`: If you create Object\_1 to 10 and remove Object\_3, "Object\_3" joins the pool of revoked ids and can be assigned to the next object to be created.

### RDFPropertyMap
The RDFPropertyMap is a utility entity that allows its user to store values of different types in a single datastructure. Internally, it uses `Soprano::LiteralValue`s (which are based on `QVariant`) to store the most common datatypes. As the name implies, it is a map-structure: String-keys are mapped to values, and since it is derived from `RDFEntity`, a triple exists for every key-value-pair. Therefore, the map has to be given a subject to use in the triples as well as a base-URI that precedes the keys to form valid properties in RDF. E.g.:
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


## Events

### EventBroker

## Processing Modules
SEMPR can be extended by adding processing modules that react to events and accept queries as you need them. Every processing module manages a mapping of ``type_index`` to functions. Whenever an event needs to be processed, the ``type_index`` of the event is looked up in the map and the respective function is called. This mechanism allows a kind of type-safe way to handle events, while being able to easily add new processing modules and events. However, there is one major drawback: Derived events have a new  ``type_index`` and hence do not trigger the functions registered for their base types.

To create a custom processing module you have to subclass from ``Module`` and register a function using ``addOverload``, as shown below:

``` c++
class CustomModule : public Module
{
public:
	CustomModule()
	{
		addOverload<SpecialEvent>(
			[this](SpecialEvent::Ptr event)
			{
				myVerySpecialProcessingMethod(event);
			}
		);
	}

	void myVerySpecialProcessingMethod(SpecialEvent::Ptr event)
	{
		// ...
	}
};
```

### Query
There are two ways of adding query-answering-capabilities to a module: The base class `Module` provides a default implementation of `virtual void Module::answer(Query::Ptr);` in which it simply calls `this->notify(query)`. This is possible because `Query`derives from `Observable` just like `Event`. Therefore you can use the `addOverload`-mechanism that has been described before to also register query-types.

#### ObjectQuery
There may be a need to handle things a bit differently: E.g., the `ActiveObjectStore` supports the `ObjectQueryBase`-type which is inherited by the templated `ObjectQuery<Entity>` - queries. Since the addOverload-mechanism only works for exact types and the ActiveObjectStore cannot know all types of entities in advance, it overrides the `answer`-method. This way it can implement the type-check using a dynamic cast and thus hande any query derived from `ObjectQueryBase`.

**TODO: This is a different way to handle the same problem as we encountered with events. For events, we decided to fire one event of every inherited type explicitly. Can one method profit from the other? Is there a way to combine this, use one consistent strategy?**

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
	- reference frame
	- semantic object
	- geometric object
	- general object (to be wrapped by use-case-classes, plus factory, to avoid the odb compiler)
- Add processing modules
	- symbolic reasoning (rdf)
	- something tf-like (use envire)
	- GeometryCache
