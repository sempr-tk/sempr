# Semantic Environment Mapping, Processing and Reasoning (SEMPR)
## Status
As the development of SEMPR just started recently, please do not expect a fully featured system. We are working on it. :)

## Installation

SEMPR currently relies on a few other libraries: ODB for the object relational mapping, sqlite3 for a database connection, boost for uuids, soprano for SPARQL queries, qt4 for soprano, and a recent version of geos and boost geometry for more geometric operations.

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
SEMPR itself is released under a 3-clause BSD license. However, it relies heavily on [ODB](http://www.codesynthesis.com/products/odb/download.xhtml) which is licensed under GPL. Please take a look at the [license exception](LICENSE_ODB) granted by Code Synthesis (copied from [here](https://git.codesynthesis.com/cgit/odb/odb-etc/tree/license-exceptions/sempr-tk-odb-license-exception.txt)) which allows the use of SEMPR (everything belonging to the sempr-tk family) in other projects without imposing additional constraints to the BSD-3-license -- as long as you don't modify the persistent object model for which ODB is used.

## Maintenance and development
DFKI GmbH - Plan-Based Robot Control

![dfki-logo](doc/dfki_logo.jpg)

## Usage

==TODO==

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
