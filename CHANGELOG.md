# Changelog

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
