#ifndef SEMPR_ENTITY_SPATIAL_GEOMETRY_HPP_
#define SEMPR_ENTITY_SPATIAL_GEOMETRY_HPP_

#include <sempr/entity/Entity.hpp>
#include <sempr/entity/spatial/SpatialReference.hpp>
#include <geos/geom/Geometry.h>

#include <type_traits>

namespace sempr
{
namespace entity
{

namespace geom = geos::geom;

/**
    A base class for all geometry entities. Contains methods to manage spatial reference systems
    but no concrete geometry -- just a pure virtual OGRGeometry* geometry() method to be implemented
    by the derived classes (point, polygon, ...).
*/
#pragma db object
class Geometry : public Entity
{
    SEMPR_ENTITY
  public:
    using Ptr = std::shared_ptr<Geometry>;

    Geometry();
    Geometry(const core::IDGenBase *);

    virtual ~Geometry();

    // Note: I'd like this to be a pure virtual, but in that case ODB does not create a
    // discriminator for the class (why should it, if no instances of it can be created?),
    // which again leads to compiler errors.
    // unable to override if method is const.
    // its not allow to use this geometry in another geometry (e.g. collection). Otherwise you will get multiple ownerships to this object!
    // To use a collection build a clone of this geometry or construct a new one.
    virtual const geom::Geometry* geometry();

    /**
     * @brief Set the GEOS GEOM geometry object.
     * The new object have to be created by the GeometryFactory!
     * 
     * @param geometry The new geometry object to set.
     */
    void setGeometry(geom::Geometry* geometry);

    /**
        Assigns this geometry to the given reference frame. No transformations will be done.
    */
    void setCS(SpatialReference::Ptr cs);

    /** returns the currently set SpatialReference */
    SpatialReference::Ptr getCS() const;

    /**
        Transforms this geometry into the new reference frame.
        \throws TransformException if the geometry cannot be transformed.
            Reasons may be:
                - no geometry
                - target/source null
                - different root CS that are not global
                - transform between two global frames not known to GDAL/proj4
    */
    void transformToCS(SpatialReference::Ptr cs); // replacement needed without the gdal dependency!

    

    /**
        Get a new entity with the same geometry (copy) referring to the same instance of
        SpatialReference. (implemented by raw_clone)
    */
    Geometry::Ptr clone() const;

    static geom::Geometry *importFromWKB(const std::basic_string<char> &buffer);
    static std::basic_string<char> exportToWKB(const geom::Geometry *geom);

    static geom::Geometry *importFromWKT(const std::string &text);
    static std::string exportToWKT(const geom::Geometry *geom);

  protected:
    static const geom::GeometryFactory *factory_;

    #pragma db type("TEXT")
    geom::Geometry* geometry_;

  private:
    friend class odb::access;    

    /**
        Return a pointer to a clone of this. Uses the copy-constructor. To be implemented by derived
        classes to copy the geometry and use the same SpatialReference (instance!).
        \return nullptr "Geometry" is just an abstract concept, no need to copy this.
    */
    virtual Geometry *raw_clone() const;

    SpatialReference::Ptr referenceFrame_;
};

} // namespace entity
} // namespace sempr

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_GEOMETRY_HPP_ */
