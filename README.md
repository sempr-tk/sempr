# Semantic Environment Mapping, Processing and Reasoning (SEMPR)

[![pipeline status](https://git.ni.dfki.de/sempr/sempr/badges/ecs/pipeline.svg)](https://git.ni.dfki.de/sempr/sempr/commits/ecs)

## Status
SEMPR is still under heavy development, so please expect things to change (and break). :)

## Installation

SEMPR currently relies on a few other libraries: ODB for the object relational mapping, sqlite3 for a database connection, boost for uuids, soprano for SPARQL queries, qt4 for soprano, and a recent version of geos and boost geometry for more geometric operations.

### GeographicLib, soprano, boost, geos 
Soprano and boost can be retrieved easily, **if you are okay with using qt4**:
```bash
sudo apt-get install libsoprano-dev qt4-default
sudo apt-get install libboost-all-dev
```
The boost-part is a bit overkill, but I don't really know which part of boost is needed. You could try `libboost-dev`...

**If you need qt5** instead, you will need a customized build of soprano: SEMPR uses Soprano, and qt to work with it. Whatever version of qt is used by soprano will also be used by SEMPR, as it is propagated through pkg-config. Compiling soprano with qt5-support can be a bit tricky, especially if you are simultaneously using `libcurl4-openssl-dev`, e.g. if you have ROS installed. But don't worry, everything is explained in detail at the wiki page on [how to enable qt5 support](https://github.com/sempr-tk/sempr/wiki/SEMPR-with-Qt5).

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



## License
SEMPR is released under a 3-clause BSD license. 

## Maintenance and development
DFKI GmbH - Plan-Based Robot Control

![dfki-logo](doc/dfki_logo.jpg)

## Usage

SEMPR is in itself not a ready-to-be-used executable, but rather a library, call it a framework if you like, that you are supposed to extend to your own needs, and integrate it into your own domain specific application.

But of course, there are some reoccurring patterns in its usage, and this is where we take it look at those.

One central part of SEMPR is the `sempr::Core`, though its functionality is quite limited. It owns an instance of `rete::Reasoner` and is responsible of notifying the it whenever data is added, removed or updated. Furthermore, it assigns an ID to the `sempr::Entity`s that are added.

```c++
#include <sempr/Core.hpp>
//...
sempr::Core core;
auto entity = sempr::Entity::create();
auto component = std::make_shared<MyComponent>(); // more on available components later

// the order of adding entities to the core and components to the entity does not matter
entity->addComponent(component);
core.addEntity();

// ... update some data of our imaginary MyComponent
component->setValue(42);
// upon internal changes of components, you need to inform the reasoner:
component->changed();
```

Note that adding and removing components from an entity as well as calling `component->changed()` all trigger the rete network that is used inside the reasoner. That means that all relevant rules are (re-)evaluated, but the rules effect do not take place. In order to perform the reasoning for real, realizing effects and all consequences of those effects, you have to explicitely call:

```c++
core.reasoner().performInference();
```

At this point, this does not make any sense, as we did not add any rules yet. Normally you would do this right at the start, before adding entities to the system, but it is also possible to add (and remove) rules after data.

Adding rules means extending the reasoners internal rete network. Since it is quite cumbersome to do this manually, we make use of a rule parser that transforms a textual representation of our rules into the pattern matching network. By default, the parser only understands basic RDD triple conditions and effects, but you can add additional node builders to extend its capabilities. E.g., to access our `MyComponent` in a rule we will need an instance of `sempr::ECNodeBuilder`:

```c++
#include <rete-reasoner/RuleParser.hpp>
#include <sempr/ECNodeBuilder.hpp>

rete::RuleParser parser;
parser.registerNodeBuilder<sempr::ECNodeBuilder<MyComponent>>();

parser.parseRules(
    "[rule1: EC<MyComponent>(?entity ?component) -> (<foo> <bar> <baz>)]",
    core.reasoner().net()
);
```

The above rule simply infers an RDF triple `<foo> <bar> <baz>` for every existing entity-MyComponent-pair.

After performing inference you might want to work with the inferred data. Theoretically, you can do this by accessing the internal state of the reasoner where you can iterate over all `rete::WME`s. But usually everything that happens with the data should be triggered by the reasoner, through a rule. Feel free to add your own effects! Even if you need to answer queries on demand, consider adding the updating logic as an effect for the reasoner. This is how it is done with the SopranoModule, which can answer SPARQL-queries on the rdf triples. In order to use it you will need to create the module itself, and a node builder that connects it with an effect of a rule:

```c++
#include <sempr/nodes/SopranoNodeBuilder.hpp>

// create the module itself
auto soprano = std::make_shared<sempr::SopranoModule>();
// create a node builder for it
std::unique_ptr<sempr::SopranoNodeBuilder> 
    builder(new sempr::SopranoNodeBuilder(soprano));

// add the node builder to the parser
parser.registerNodeBuilder(std::move(builder));

// use it in a rule
parser.parserRules(
    "[updateSoprano: (?s ?p ?o) -> SopranoModule(?s ?p ?o)]",
    core.reasoner().net()
);
```

This rule makes sure that all the triples are added to the soprano module (you could even limit it to certain triples, or manipulate them before adding them, if you so desire for whatever reason). Now (after `performInference()`, of course) you can let soprano answer your queries.

```c++
sempr::SPARQLQuery query;
query.query = "SELECT * WHERE { ?a <ex:foobar> ?b . }";
soprano->answer(query);
for (auto result : query.results)
{
    // ...
}
```

## Implemented functionality (so far)

==TODO: Find a nice way to present the syntax of the implemented builtins==

### Components

- TripleCointainer (interface), TripleVector
  Contain rdf triples.
- GeosGeometry
  Wraps a `geos::geom::Geometry*`
- AffineTransform
  Wraps a `Eigen::Affine3d` (access in rules is currently limited to translation and rotation)

### Nodes/Builtins

| syntax                                    | description                                                  | node builder                   | include                           |
| ----------------------------------------- | ------------------------------------------------------------ | ------------------------------ | --------------------------------- |
| `EC<ComponentName>( ?entity ?component )` | **Condition.** Matches entities ?entity that have a component ?component of type ComponentName. _Note: ComponentName == sempr::ComponentName\<ComponentType\>::value_ | `ECNodeBuilder<ComponentType>` | nodes/ECNodeBuilder.hpp           |
| `ExtractTriples(?c)`                      | **Effect.** Extracts the triples from the TripleContainer ?c for direct usage in the reasoner. | `ExtractTriplesBuilder`        | nodes/ExtractTriplesBuilder.hpp   |
| `SopranoModule(?s ?p ?o)`                 | **Effect.** Updates the underlying SopranoModule by adding the triple (?s ?p ?o). | `SopranoNodeBuilder`           | nodes/SopranoNodeBuilder.hpp      |
| `tf:create(?tf ?x ?y ?z ?qx ?qy ?qz ?qw)` | **Builtin.** Creates a new AffineTransform ?tf with the given parameters for translation (?x ?y ?z) and rotation (as quaternion) (?qx ?qy ?qz ?qw). | `AffineTransformCreateBuilder` | nodes/AffineTransformBuilders.hpp |
| `tf:get(?tf ?x [?y ?z ?qx ?qy ?qz ?qw])`  | **Builtin.** Extracts the parameters from the given AffineTransform ?tf. ?y to ?qw are optional. | `AffineTransformGetBuilder`    | nodes/AffineTransformBuilders.hpp |
| `tf:mul(?result ?left ?right)`            | **Builtin.** Binds ?result to ?left * ?right, where ?left and ?right must be bound to AffineTransforms. | `AffineTransformMulBuilder`    | nodes/AffineTransformBuilders.hpp |
| `tf:inv(?result ?input)`                  | **Builtin.** Binds ?result to the inverse of the AffineTransform given in ?input. | `AffineTransformInvBuilder`    | nodes/AffineTransformBuilders.hpp |



## Internal structure
SEMPR follows an Entity-Component-System architecture: Information is stored in instances of `sempr::Entity`, which can be identified by a unique id. Entities are actually nothing more than that: An ID together with a list of `sempr::Component`, where the components can be geometries, rdf triples, or any other custom blob of data.

So, what can we do with this list of entities, containing lists of components? This is where the "System" part comes into play. Systems work on specific parts of the data to implement specific functionalities. Think of computing new information, updating it due to changes in the underlying data, sending notifications to external systems (e.g. ROS, ...). The interesting part is how the systems and data interact: The core of SEMPR is a [rule based forward reasoner based on an implementation of the rete algorithm](http://git.ni.dfki.de/sempr/rete). Entity-Component-pairs are added as the working memory elements of this reasoner, in addition to rdf triples representing semantic information of the world state. This way, you can implement and use custom computations/checks on the components in your (probably domain specific) rule definitions. So, the "Systems" part are the effects of the rules the reasoner executes.

Having an actual reasoner as the core of SEMPR allows us to automatically retract data that was inferred from facts that no longer hold, update only the things that need updating, and inform the effects of rules whenever a previous match for the preconditions no longer holds. 

## Extensibility

This library is designed to be heavily extended for your personal needs. You might want to:

- Implement custom components, to add custom datatypes.
  - In this case, also register a `sempr::ECNodeBuilder<YourComponent>` at the rule parser to be able to add existence conditions for your components your rules, e.g.
    `[rule1: EC<YourComponent>(?entity ?component), ... -> ...]`
    Where "YourComponent" is the name you specified for the trait `sempr::ComponentName<YourComponent>::value`.
- Add new conditions / computations for a component type. For this you will want to create a new type of node for the rete network in the reasoner, which is derived from `rete::Builtin`.
  - You will also need a new `rete::NodeBuilder` that creates the node from its textual representation in your rules.
- Add new working memory elements beside triples and entity-component-pairs, things that could be e.g. computed and returned by your builtins. Subclass from `rete::WME`. 
  - And don't forget to also implement a `rete::Accessor` for it.
- Add new "Systems", new effects to trigger when a complete match for the preconditions in your rules is found. Subclass from `rete::Production` .
  - And again, don't forget you will need to implement a new `rete::NodeBuilder` and register it at the rule parser.

Please refer to the documentation of the `rete::Reasoner` for a more comprehensive and detailed description.



### Geometry
To store geometric data, SEMPR relies on [GEOS](http://geos.osgeo.org), which implements a standard proposed by the _Open Geospatial Consortium (OGC)_. 

==TODO==


#### Serialization with ODB
The `traits-sqlite-geom-geometry.hxx` implements a traits-class for `geos::geom::Geometry*` with templated methods to store any pointer to an `geos::geom::Geometry` or derived class. The serialization support is easily extended by inheriting from this traits-class: E.g., the line:

```c++
template <> class value_traits<geom::Point*, id_blob> : public value_traits<geom::Geometry*, id_blob> {};
```

enables serialization of `geos::geom::MultiPoint*` in binary form (WKB). Use, e.g.:
```c++
#pragma db type("BLOB")
geos::geom::MultiPoint* geometry_;
```
