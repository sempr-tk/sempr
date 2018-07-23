#ifndef SEMPR_ENTITY_SPATIAL_PROJECTIONCS_HPP_
#define SEMPR_ENTITY_SPATIAL_PROJECTIONCS_HPP_

#include <sempr/entity/spatial/reference/GlobalCS.hpp>
#include <sempr/entity/spatial/filter/ProjectionFilter.hpp>
#include <GeographicLib/UTMUPS.hpp>
#include <string>

namespace sempr { namespace entity {

// Forward Declaration of derived classes to allow import/export
class UniversalTransverseMercatorCS;

/**
    A projected coordinate system, using a base-geographic system (e.g. WGS84) and a
    projection into cartesian space. It is also a root of transformations (does not have a parent).
*/
#pragma db object
class ProjectionCS : public GlobalCS {
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<ProjectionCS>;

    virtual FilterList to(const GlobalCS::Ptr other);

    // import or export a UTM/UPS zone string
    static ProjectionCS::Ptr importZone(const std::string& zonestr);
    static std::string exportZone(const ProjectionCS::Ptr cs);

    // import or export a UTM/UPS zone EPSG code.
    static ProjectionCS::Ptr importEPSGZone(int epsg);
    static int exportEPSGZone(const ProjectionCS::Ptr cs);

protected:

    virtual FilterPtr forward() const;
    virtual FilterPtr reverse() const;

    ProjectionCS();
    ProjectionCS(const core::IDGenBase*);

private:
    friend class odb::access;
};


}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_PROJECTIONCS_HPP_ */
