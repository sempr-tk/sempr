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

## Processing Modules
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