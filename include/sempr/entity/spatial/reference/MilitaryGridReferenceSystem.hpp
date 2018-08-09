#ifndef SEMPR_ENTITY_SPATIAL_MILITARYGRIDREFERENCESYSTEM_HPP_
#define SEMPR_ENTITY_SPATIAL_MILITARYGRIDREFERENCESYSTEM_HPP_

#include <sempr/entity/spatial/reference/ProjectionCS.hpp>
#include <sempr/entity/spatial/filter/ProjectionFilter.hpp>

namespace sempr { namespace entity {

/**
 *  Coordinate system of a local tangent plane of a given origin (x/y/z) position.
 *  Use WGS84 as reference ellipsoid.
*/

#pragma db object
class MilitaryGridReferenceSystem : public ProjectionCS {
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<MilitaryGridReferenceSystem>;

    MilitaryGridReferenceSystem(const std::string& GZDSquareID);    //eg. 4QFJ
    MilitaryGridReferenceSystem(int zone, char designator, const std::string& squareID);
    MilitaryGridReferenceSystem(int zone, char designator, const std::string& squareID, const core::IDGenBase*);

    bool isEqual(const GlobalCS::Ptr other) override;

    int getZone() const override;
    bool isNorth() const override;

    std::string getGZDSquareID() const;

    static bool checkGZD(const std::string& GZDSquareID);
    static std::string buildMGRS(const std::string& GZDSquareID, double x = 0, double y = 0, const std::string& separator = " ");
    static void splitMGRS(const std::string& mgrs, std::string& GZDSquareID, double& x, double& y);

protected:
    MilitaryGridReferenceSystem();

    virtual FilterPtr forward() const override;

    virtual FilterPtr reverse() const override;

    static int zoneFromGZD(const std::string& GZDSquareID);
    static char designatorFromGZD(const std::string& GZDSquareID);
    static std::string squareIDFromGZD(const std::string& GZDSquareID);


private:
    friend class odb::access;

    const int zone_;
    const char designator_;
    const std::string squareID_;

    static bool isLetter(char c);
    static bool isNumber(char c);
};


}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_MILITARYGRIDREFERENCESYSTEM_HPP_ */
