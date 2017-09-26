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
**TODO**: _Rename sempr to sempr_
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
The basic event-handler provides two base-implementations to cope with events:
``` c++
virtual void process(Event::Ptr); // for custom events
virtual void process(EntityEvent::Ptr); // for events that refer to a specific entity
```
To handle additional events, override one of those methods and use ``dynamic_pointer_cast`` to check the type of event.

**TODO:** _Is there no better way to handle this, make it static-type-safe and extendable? (e.g. double dispatch using visitor pattern, type safe but not extendable.)_

### EventBroker