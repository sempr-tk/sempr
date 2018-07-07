#ifndef SEMPR_ENTITY_POINTCLOUD_HPP_
#define SEMPR_ENTITY_POINTCLOUD_HPP_

#include <sempr/storage/History.hpp>
#include <sempr/entity/Entity.hpp>
#include <sempr/entity/RDFPropertyMap.hpp>
#include <memory>

#include <type_traits>

namespace sempr { namespace entity {

#pragma db object
class PointCloud : public Entity
{
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<PointCloud>;
    //ENTITY_DEFAULT_EVENT_METHODS(PointCloud, sempr::entity::Entity)

    PointCloud();
    PointCloud(const sempr::core::IDGenBase*);

    // TODO: The Pointcloud internal structure ...
    // maybe we save Points here? Of cause we do, but how?

    // So such a propertymap can't be used with double*, ...
    // so I need to make each bound its own property.

    uint64_t number() const { return (uint64_t)((*prop_)["number"]); }
    bool color() const { return (bool)((*prop_)["color"]); }
    std::shared_ptr<double> bounds() const;
    std::shared_ptr<double> points() const { return m_points; }
    //double* points() const { return m_points.get(); }
    std::shared_ptr<unsigned char> colors() const { return m_colors; }
    //unsigned char* colors() const { return m_colors.get(); }

    void setPointsWithColor(std::shared_ptr<double>& points,
                            std::shared_ptr<unsigned char>& colors,
                            uint64_t n);

    
    virtual ~PointCloud(){}

protected:
    friend class odb::access;

    // we should not be able to set the bounds manually - let sempr calculate them
    void bounds(double *b);
    void bounds(std::shared_ptr<double> b);
    // seems, that we can't use ulong in RDF (Soprano)
    void number(uint64_t n) { (*prop_)["number"] = (qulonglong)n; changed(); prop_->changed(); }
    void color(bool c) { (*prop_)["color"] = (bool)c; changed(); prop_->changed(); }

    //this arrays are for the points and the colors
    std::shared_ptr<double> m_points;
    std::shared_ptr<unsigned char> m_colors;
    
    RDFPropertyMap::Ptr prop_;
};

// enable history:
typedef storage::History<PointCloud::Ptr> PointCloudHistory;
#pragma db value(PointCloudHistory)

}}

#endif /* end of include guard: SEMPR_ENTITY_POINTCLOUD_HPP_ */
