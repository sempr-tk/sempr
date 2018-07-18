#ifndef SEMPR_ENTITY_SPATIAL_PROJECTIONCS_HPP_
#define SEMPR_ENTITY_SPATIAL_PROJECTIONCS_HPP_

#include <sempr/entity/spatial/reference/GlobalCS.hpp>
#include <sempr/entity/spatial/filter/ProjectionFilter.hpp>
#include <GeographicLib/UTMUPS.hpp>
#include <string>

namespace sempr { namespace entity {

/**
    A projected coordinate system, using a base-geographic system (e.g. WGS84) and a
    projection into cartesian space. It is also a root of transformations (does not have a parent).
*/
#pragma db object
class ProjectionCS : public GlobalCS {
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<ProjectionCS>;

    static const int AUTO_ZONE = GeographicLib::UTMUPS::UTM;

    /**
     * @brief Get the UTM Zone of this coordinate reference system
     * 
     * Will throw an invalid_argument exception if this is called on a MGRS Projection
     * 
     * @return int The UTM zone (e.g. 32)
     */
    int getUTMZone() const;
    void setUTMZone(int zone = AUTO_ZONE);

    bool getNorth() const;
    void setNorth(bool north);

    /**
     * @brief Set the zone and part of hemisphere based on the given position
     * 
     * @param lat in degree
     * @param lon in degree
     */
    void setRegion(double lat, double lon);

    /**
        Create a ProjectionCS for a given UTM zone and a base geographic system.
        NOTE: This is not the UTM you know. This is bare UTM, i.e. the world sliced in 6° longitude and split in north / south. What you might know as e.g. UTM zone 32U is an extension to that, called MGRS (military grade reference system), sometimes known as UTMREF, which introduces finer grained zones. This is not supported right now -- BUT: GDAL has an undocumented feature for MGRS, take a look at frmts/nitf/mgrs.c, maybe you can use that in some way.
        \param zone UTM zone
        \param base geographic system to use, e.g. WGS84
        \return empty ptr (NULL) on error, else a new ProjectionCS-entity.
    */
    static ProjectionCS::Ptr CreateUTM(int zone = AUTO_ZONE);

    /**
     * @brief 
     * 
     * @param north true if on northern hemisphere
     * @param reference 
     * @return ProjectionCS::Ptr 
     */
    static ProjectionCS::Ptr CreateUPS(bool north = true);

    /**
     * @brief Create a MGRS Projection for a given zone
     * 
     * The MGRS format use WGS84 as base.
     * By default the north spherical is set but in absolute it depends on the zone and will be corrected by the transformation
     * 
     * @param zone MGRS zone like 38SMB or 32U
     * @return ProjectionCS::Ptr 
     */
    static ProjectionCS::Ptr CreateMGRS(const std::string& zone);

protected:
    ProjectionCS(double k0, const std::string& zone, bool north);
    ProjectionCS(double k0, const std::string& zone, bool north, const core::IDGenBase*);

//    virtual std::shared_ptr<geos::geom::CoordinateFilter> froward() const;

//    virtual std::shared_ptr<geos::geom::CoordinateFilter> reverse() const;

private:
    friend class odb::access;

    ProjectionCS(); // just for ODB.

    double k0_;     // central scale factor
    std::string zone_;      // zone of utm projection
    bool north_;    // north spherical

};


}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_PROJECTIONCS_HPP_ */
