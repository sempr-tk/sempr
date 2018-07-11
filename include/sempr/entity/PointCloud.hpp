#ifndef SEMPR_ENTITY_POINTCLOUD_HPP_
#define SEMPR_ENTITY_POINTCLOUD_HPP_

#include <sempr/storage/History.hpp>
#include <sempr/entity/Entity.hpp>
#include <sempr/entity/RDFPropertyMap.hpp>
#include <memory>
#include <vector>

#include <type_traits>

namespace sempr { namespace entity {


#pragma db object
/**
 * @brief The PointCloud class is a Entity that represents a Pointcloud
 */
class PointCloud : public Entity
{
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<PointCloud>;

    PointCloud();
    PointCloud(const sempr::core::IDGenBase*);

    // TODO: The Pointcloud internal structure ...
    // maybe we save Points here? Of cause we do, but how?

    // So such a propertymap can't be used with double*, ...
    // so I need to make each bound its own property.

    uint64_t number() const { return (uint64_t)((*prop_)["number"]); }
    bool color() const { return (bool)((*prop_)["color"]); }
    std::shared_ptr<double> bounds() const;

    // what should we return?
    //std::vector<double> points() { return m_points.points(); }
    std::shared_ptr<double> points() { return std::shared_ptr<double>(m_points->points().data(), std::default_delete<double[]>()); }
    //double* points() const { return m_points->points().data(); }

    //std::vector<unsigned char> colors() { return m_colors.colors(); }
    std::shared_ptr<unsigned char> colors() { return std::shared_ptr<unsigned char>(m_points->colors().data(), std::default_delete<unsigned char[]>()); }
    //unsigned char* colors() const { return m_colors.->points().data(); }

    void setPointsWithColor(std::shared_ptr<double>& points,
                            std::shared_ptr<unsigned char>& colors,
                            uint64_t n);

    
    virtual ~PointCloud(){}

    /**
     * @brief The Points class to access individual Points in the Cloud
     */
    class Points
    {
        using Ptr = std::shared_ptr<Points>;

        Points(uint64_t n, PointCloud* pc) : m_pc(pc)
        {
            m_pc->number(n);

            mm_points.reserve(n * 3);
            mm_colors.reserve(n * 3);
        }

        std::vector<double> mm_points;
        std::vector<unsigned char> mm_colors;

        #pragma db transient
        PointCloud* m_pc;

    public:
        // how can I make this save?
        void points(const std::shared_ptr<double> p)         { std::fill(mm_points.begin(), mm_points.end(), *(p.get())); }
        void points(const double*& p)                        { std::fill(mm_points.begin(), mm_points.end(), *p); }
        void points(const std::vector<double>& p)            { this->mm_points = p; m_pc->number(p.size()); }

        void colors(const std::shared_ptr<unsigned char> c)  { std::fill(mm_colors.begin(), mm_colors.end(), *(c.get())); }
        void colors(const unsigned char*& c)                 { std::fill(mm_colors.begin(), mm_colors.end(), *c); }
        void colors(const std::vector<unsigned char>& c)     { if(c.size() == 3 * m_pc->number()) this->mm_colors = c; } // else?

        std::vector<double> points() { return mm_points; }
        std::vector<unsigned char> colors() { return mm_colors; }

        // return single point i
        int p(uint64_t i, std::vector<double>& p)
        {
            if (p.size() != 3)
                p.resize(3);
            if( (this->x(i, p[0]) == 0) && (this->y(i, p[1]) == 0) && (this->z(i, p[2]) == 0))
                return 0;
            return -1;
        }
        // return single color of point i
        int c(uint64_t i, std::vector<unsigned char>& c)
        {
            if (c.size() != 3)
                c.resize(3);
            if( (this->r(i, c[0]) == 0) && (this->g(i, c[1]) == 0) && (this->b(i, c[2]) == 0))
                return 0;
            return -1;
        }


        // single value of point
        int x(uint64_t i, double& x) { return  w(i, x, 0); }
        int y(uint64_t i, double& y) { return  w(i, y, 1); }
        int z(uint64_t i, double& z) { return  w(i, z, 2); }
        int w(uint64_t i, double& w, uint64_t j)
        {
            if (i < m_pc->number())
            {
                w = mm_points[(i * 3) + j];
                return 0;
            }
            return -1;
        }

        // single color
        int r(uint64_t i, unsigned char& r) { return  a(i, r, 0); }
        int g(uint64_t i, unsigned char& g) { return  a(i, g, 1); }
        int b(uint64_t i, unsigned char& b) { return  a(i, b, 2); }
        int a(uint64_t i, unsigned char& a, uint64_t j)
        {
            // check for mm_colors.emtpy() ? or check for i < mm_colors.capacity() ?
            if (i < m_pc->number())
            {
                a = mm_colors[(i * 3) + j];
                return 0;
            }
            return -1;
        }
    };

protected:
    friend class odb::access;

    // we should not be able to set the bounds manually - let sempr calculate them
    void bounds(double *b);
    void bounds(std::shared_ptr<double> b);
    // seems, that we can't use ulong in RDF (Soprano)
    void number(uint64_t n) { (*prop_)["number"] = (qulonglong)n; changed(); prop_->changed(); }
    void color(bool c) { (*prop_)["color"] = (bool)c; changed(); prop_->changed(); }

    //this arrays are for the points and the colors
    //std::shared_ptr<double> m_points;
    //std::shared_ptr<unsigned char> m_colors;
    #pragma db type("BLOB")
    Points* m_points;

    void points(const std::shared_ptr<double>& p)            { m_points->points(p); }
    void points(const double*& p)                            { m_points->points(p); }
    void points(const std::vector<double>& p)                { m_points->points(p); }

    void colors(const std::shared_ptr<unsigned char>& c)     { m_points->colors(c); }
    void colors(const unsigned char*& c)                     { m_points->colors(c); }
    void colors(const std::vector<unsigned char>& c)         { m_points->colors(c); }
    
    RDFPropertyMap::Ptr prop_;
};

// enable history:
typedef storage::History<PointCloud::Ptr> PointCloudHistory;
#pragma db value(PointCloudHistory)

}}

#endif /* end of include guard: SEMPR_ENTITY_POINTCLOUD_HPP_ */
