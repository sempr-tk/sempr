# Semantic Mapping, Processing and Reasoning (SEMPR)
## Status
As the development of SEMPR just started recently, please do not expect a fully featured system. We are working on it. :)

## Installation
One main dependency of SEMPR is the object relational mapping framework [ODB](http://www.codesynthesis.com/products/odb/download.xhtml), which uses a special compiler to create the necessary database code.

You will need:

- ODB Compiler (odb-2.4.0)
- Common Runtime LIbrary (libodb-2.4.0)
- Database Runtime Libraries
	- libodb-sqlite-2.4.0
	- ... more as soon as different database backends are implemented.
- Profile Libraries
	- libodb-boost-2.4.0

Under Ubuntu (>=16.04) you can install those as prepackaged version.

```
sudo apt-get install odb libodb-dev libodb-sqlite-2.4 libodb-boost-2.4
```

Follow the respecting installation instructions. Afterwards, compile SEMPR using``cmake``:


``` bash
mkdir build
cd build
cmake ..
make
```

## License
SEMPR itself is released under a 3-clause BSD license. However, it relies heavily on [ODB](http://www.codesynthesis.com/products/odb/download.xhtml) which, without a license exception, is licenced under GPL.

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
By using an `RDFPropertyMap` other entites are able to store their information and provide them in an RDFEntity without much redundancy and overhead. Whether you want to use the `RDFPropertyMap` exclusively or just use it to store otherwise transient data members in (during prePeresist/preUpdate/postLoad) is up to you -- as long as the map gets the values they are available to (optional, not yet implemented) reasoners.

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

There may be a need to handle things a bit differently: E.g., the `ActiveObjectStore` supports the `ObjectQueryBase`-type which is inherited by the templated `ObjectQuery<Entity>` - queries. Since the addOverload-mechanism only works for exact types and the ActiveObjectStore cannot know all types of entities in advance, it overrides the `answer`-method. This way it can implement the type-check using a dynamic cast and thus hande any query derived from `ObjectQueryBase`.

**TODO: This is a different way to handle the same problem as we encountered with events. For events, we decided to fire one event of every inherited type explicitly. Can one method profit from the other? Is there a way to combine this, use one consistent strategy?**



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
