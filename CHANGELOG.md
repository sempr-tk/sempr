# Changelog


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
