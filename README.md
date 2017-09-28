# Semantic Mapping, Processing and Reasoning (SEMPR)
## Installation
One main dependency of SEMPR is the object relational mapping framework [ODB](http://www.codesynthesis.com/products/odb/download.xhtml), which uses a special compiler to create the necessary database code.

You will need:

- ODB Compiler (odb-2.4.0)
- Common Runtime LIbrary (libodb-2.4.0)
- Database Runtime Libraries
	- libodb-sqlite-2.4.0
	- more to be announced. ;) 
- Profile Libraries
	- libodb-boost-2.4.0
  
  Follow the respecting installation instructions. Afterwards, compile SEMPR using``cmake``:
  
``` bash
mkdir build 
cd build
cmake ..
make 
```

## Usage
``` c++
#include <sempr/sempr.h>
using namespace sempr::storage;
using namespace sempr::entity;
using namespace sempr::core;

int main(int argc, char** args)
{
	// create a storage module
	ODBStorage::Ptr storage( new ODBStorage() );
	
	// create the core structure of sempr
	Core core(storage);
	
	// create processing modules that react on events
	DebugModule::Ptr debug( new DebugModule() );
	core.addModule(debug);
	
	// CoffeeMug::Ptr mug1( new CoffeeMug() );
	core.addEntity(mug1);	// triggers an EntityEvent
	
	return 0;
}
```
## Internal structure
SEMPR consists of a collection of different `Entity`s (things that can be stored in the database) and processing modules. Whenever a something changed and the rest of the system needs to be informed, an event can be delivered to the processing modules. Typically, an event stores a pointer to the entity that changed, and the processing modules subscribe to specific types of events (see [processing modules](#processing)). 

Events are the way automatic (forward) processing is implemented: Whenever something changed, all processing modules get the chance to react to the change, update their internal datastructure, modify data and trigger more events, etc. Since some calculations might be very cost-intensive, methods exist to create information on-demand (backward processing/reasoning): Whenever a `Query` is to be answered, not only the currently existing data is checked for an answer, but the query itself is handed to the processing modules in order to extend the results. while processing a query, the modules may create sub-queries of their own. This exhibits a risk of endless loops -- later implementations might introduces clever strategies to cope with that, e.g. a depth-limit.
**TODO:** _This has not been implemented yet._

The main focus of processing modules lies on symbolic and spatial reasoning. Hence, interfaces are needed as to provide those information, symbolic knowledge and geometries, in an orderly manner. Therefore, currently two subclasses of `Entity` are proposed: `RDFEntity` and `GeometricEntity`. Since many entities may provide both types of data, one could try to inherit from both classes. One problem that arises in such a situation is that virtual inheritance is not supported by the underlying database system, which heavily relies on static casts (and one cannot static-cast through a virtual base). *You have been warned.* The preferred method way of dealing with this is to use `RDFEntity` and `GeometricEntity` inside your classes, instead of inheriting from them. Moreover, there may be different implementations, e.g. `MeshEntity` or `PointEntity`. See the diagram below.

![alt](doc/Entity-Structure.png)

Care must be taken that every relevant type of event is fired when an entity is created / loaded / changed or erased. When entities are added to the core, they are given a access to the event-broker, so that they are able to inform the system about changes. The question of _entity-ownership_ is a hard one -- who creates (i.e.: persists), updates and removes an entity? This is left to the user for now. Allowing an entity to create other entities is a difficult task: The entity needs access to the core (and/or database?), and take care that no pointers to non-persisted entities are to be saved. Therefore we will for now assume that the user creates and adds all entities to the core, and only creates the connections afterwards (e.g., first adding a mesh and an object, and calling something alike `object->setMesh(mesh)`afterwards. `setMesh`should, in turn, save the pointer to the mesh and call `changed()`, a method that has to be overriden by entity-types to fire special events.)


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
## Events
### EventBroker

## <a id="processing"></a> Processing Modules
SEMPR can be extended by adding processing modules that react to events (**TODO:** _and accept queries_) as you need them. Every processing module manages a mapping of ``type_index`` to function. Whenever a event needs to be processed, the ``type_index`` of the event is looked up in the map an the respective function is called. This mechanism allows a kind of type-safe way to handle events, while being able to easily add new processing modules and events. However, there is one major drawback: Derived events have a new  ``type_index`` and hence do not trigger the functions registered for their base types.

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
If you just want to process specific types of entities

## Pitfalls
### Entity creation
The base `Entity` inherits from `std::enable_shared_from_this<Entity>` to enable entities to create smart pointers to themselves, which is needed e.g. in the `Entity::changed()` method, which creates a `EntityEvent` pointing to itself and fires it through the `EventBroker` that it has been given from the `Core`. **Care** must be taken that `shared_from_this()` is only called after a smart-pointer to the object has already been created! Which in turn means that you should not create entities on the stack, but always into smart pointers:

``` c++
Entity::Ptr e1( new Entity() );
// or even better:
Entity::Ptr e2 = std::make_shared<Entity>();
```

## TODO
- Load subset from database
	- Be aware of the session caching everything that's loaded!
- Add entities
	- reference frame
	- semantic object
	- geometric object
	- general object (to be wrapped by use-case-classes, plus factory, to avoid the odb compiler)
- Add processing modules
	- entity queries
	- symbolic reasoning (rdf)
	- something tf-like (use envire)
	- GeometryCache