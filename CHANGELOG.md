# Changelog

## [0.14.0] - 2021-06-28

- create temporary (i.e., non-persistent) entities with Entity::createTemporary
- added `EC<Component>` to UtilBuildin

## [0.13.0] - 2021-05-31

- changed plugin structure to separate plugin instances for multiple sempr cores
- added `componentQuery` to RDFPlugin: An extended SPARQL query that also
  fetches c++ objects associated with ids
- added builtins to deal with dates

## [0.12.0] - 2021-03-15

- added createEntity builtin
- added idIsURI flag to entity, to allow skipping the automatic "sempr:" prefix
- added geometry builtins:
  - geo:buffer
  - geo:difference
  - geo:fromWKT
  - geo:toWKT
  - geo:WGSFromUTM

## [0.11.0] - 2021-02-19

- added geo:intersects builtin
- added createEntity builtin
- load plugins from SEMPR\_PLUGINS\_PATH env variable
- changed ECWME to include tags. This allows inferring a tag in rules

## [0.10.0] - 2020-11-19

- Added several geometry builtins:
  - geo:area
  - geo:intersection
  - geo:union
  - geo:unionAll (union on GROUP BY results)

## [0.9.3] - 2020-10-14

- Added (template) method to access loaded plugins

## [0.9.2] - 2020-10-09

- Fix: LoadTriplesFromFile builtin support for constants
- (Mainly releasing this to trigger a build with rete 0.7.0)

## [0.9.1] - 2020-09-04

- GeoDistanceNode now returns float values, for compatibility with rete
  comparison builtins (le, ge, lt, gt, eq, neq).
  At some point I should probably change it all to doubles...
- Fixed: URI representation of entities is now properly returned in an
  interpretation of rete::TriplePart (previously: std::string), whereas the
  std::string interpretation now returns the pure id.
- For compatibility with newer geos versions, the geometry component now
  contains a unique\_ptr to geos::geom::Geometry.

## [0.9.0] - 2020-09-01

- Added: Plugins that register node builders and default rules
- Fixed/Updated: Using rete v0.6.0 with a new accessor system and fixed
  string/triple handling.
- Requires C++14 now.

## [0.8.2] - 2020-07-31

- Fix: Removed leftover -std=c++17 flag from and added missing flag to link
  boost\_system to the pkg config file
- Updated/fixed: sempr.hpp had some old includes and missed some new ones
- Updated README

## [0.8.1] - 2020-05-27

- Changed install path of the library, in order to not having to mess with
  LD\_LIBRARY\_PATH. (It's <install-dir\>/lib/libsempr\_core.so now, instead of
  <install-dir\>/lib/sempr/libsempr\_core.so)

## [0.8.0] - 2020-05-12

- Added FindCereal.cmake
- Fix/Adjustment: Let the core explicitely handle rules. This is a change done
  together with rete v0.3.0 since which the RuleParser no longer adds the
  production nodes to the rete::Network, but instead returns Rule-objects which
  when deconstructed also remove the respective nodes from the network.
  This fix keeps those alive.
- Added ConstructRules-Node. This allows us to handle rules as data, by only
  hard-coding a single rule (or a very limited set of rules) that has a
  ConstructRules-effect and thus creates/removes rules as a result of the
  inference process.
- Added a simple TextComponent (in order to store rules as data).
- Added toString method for ECNode. (Requires rete v0.4.0)


## [0.7.0] - 2020-04-24

- Fix: Wrong relative include path in a header file
- Fix: Added a specialization of rete::util::to\_string for GeosGeometryInterface
- EntityAccessor now inherits StringAccessor, in which case it returns the
  entity id in a URI format, i.e.: `Entity_1` -> `<sempr:Entity_1>`. This allows
  an easy usage in rules without an extra builtin to extract the id.
- Added geo:distance(?dist ?geo1 ?geo2)
- Added geo:UTMFromWGS(?geoOut ?geoIn ?utmZone) which transforms a geometry that
  is specified in WGS84 lon/lat coordinates into a UTM coordinate system, in
  which e.g. distance calculations with geo:distance make sense.

## [0.6.0] - 2020-04-08

- Added method to TriplePropertyMapEntry that returns the current type
- Added loadFromJSON and saveToJSON methods to component. These are implemented
  in a `SEMPR_COMPONENT` macro that needs to be added to all components

## [0.5.1] - 2020-03-27

- Fix: Multiple wrong include paths
- Fix: Some ComponentName defs were missing
- Fix: ECAccessor now registers its type, making it usable in rules/builders

## [0.5.0] - 2020-03-24

- Removed dependency to ODB
- Added serialization with cereal
- Added SeparateFileStorage module

## [0.4.0] - 2020-03-23

- Added components and nodes to use with RDF triples:
  TripleDocument, TriplePropertyMap
- Added FileWatcher and a corresponding file:exists builtin to trigger rules
  when a local file changes
- Changed gitlab-ci to use a qt5 build of soprano

## [0.3.1] - 2020-03-05

There has been a complete rework of the internal structure.
Entities are only containers for components, which in turn
represent (independent) data types. The core of sempr is a
rule based rete reasoner, and entity-component pairs are made
accessible to it. What has former been thought of as a
processing module is now supposed to be implemented in a rule,
as a combination of complex preconditions (previously implemented
by listening to entity-events) and the rules effect (previously
the body of the processEvent method).

## [0.2.0] - 2018-08-09

### Added
- SemanticEntity for typesafe RDF-exposure of member variables
- RDFVector (previously RDFEntity)

### Changed
- GEOS as replacement for GDAL
- Global coordinate systems now based on geographiclib
- RDFEntity is now an interface for iterable RDF containers
- registerChildEntity now public

### Fixed
- Problems with accessing/comparing RDFPropertyMap-entries
- libsqlite3-dev dependency in CMakeLists

## [0.1.0] - 2018-07-06 - Initial Release with GDAL support.
