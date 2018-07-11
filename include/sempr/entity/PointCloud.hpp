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

        /**
         * @brief Points stored in a struct
         */
        struct Points
        {
            std::vector<double> points;
            std::vector<unsigned char> colors;
        };
        
        uint64_t number() const { return (uint64_t)((*prop_)["number"]); }
        bool color() const { return (bool)((*prop_)["color"]); }
        std::shared_ptr<double> bounds() const;
        
        // what should we return? Templating?
        std::vector<double> points() { return m_points.get()->points; }
        //std::shared_ptr<double> points() const { return std::shared_ptr<double>(m_points.get()->points.data(), std::default_delete<double[]>()); }
        //double* points() const { return points().data(); }
        
        std::vector<unsigned char> colors() { return m_points.get()->colors; }
        //std::shared_ptr<unsigned char> colors() const { return std::shared_ptr<unsigned char>(m_points.get()->colors.data(), std::default_delete<unsigned char[]>()); }
        //unsigned char* colors() const { return points().data(); }
        
        //void setPointsWithColor(std::shared_ptr<double>& points, std::shared_ptr<unsigned char>& colors, uint64_t n);
        void setPointsWithColor(std::vector<double>& points, std::vector<unsigned char>& colors, uint64_t n);
        
        virtual ~PointCloud(){}
        
        // #passt.
        
        // return single point i
        int p(uint64_t i, std::vector<double>& p)
        {
            if (p.size() != 3)
                p.resize(3);
            if( (this->x(i, p[0]) == 0) && (this->y(i, p[1]) == 0) && (this->z(i, p[2]) == 0))
                return 0;
            return -1;
        }
        // single value of point
        int x(uint64_t i, double& x) { return  w(i, x, 0); }
        int y(uint64_t i, double& y) { return  w(i, y, 1); }
        int z(uint64_t i, double& z) { return  w(i, z, 2); }
        int w(uint64_t i, double& w, uint64_t j)
        {
            if (i < this->number())
            {
                w = m_points.get()->points[(i * 3) + j];
                return 0;
            }
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
        // single color
        int r(uint64_t i, unsigned char& r) { return  a(i, r, 0); }
        int g(uint64_t i, unsigned char& g) { return  a(i, g, 1); }
        int b(uint64_t i, unsigned char& b) { return  a(i, b, 2); }
        int a(uint64_t i, unsigned char& a, uint64_t j)
        {
            // check for mm_colors.emtpy() ? or check for i < mm_colors.capacity() ?
            if (i < this->number())
            {
                a = m_points.get()->colors[(i * 3) + j];
                return 0;
            }
            return -1;
        }
        
    protected:
        friend class odb::access;
        
        // we should not be able to set the bounds manually - let sempr calculate them TODO
        void bounds(double *b);
        void bounds(std::shared_ptr<double> b);

        void number(uint64_t n) { (*prop_)["number"] = (qulonglong)n; changed(); prop_->changed(); }
        void color(bool c) { (*prop_)["color"] = (bool)c; changed(); prop_->changed(); }
        
        // how can I make this save?
        /**
         * Set the Points
         */
        void points(const std::shared_ptr<double>& p)        { m_points.get()->points = std::vector<double>(*p.get(), *(p.get() + this->number() * 3)); }
        //void points(const std::shared_ptr<double> p)         { std::fill(m_points.get()->points.begin(), m_points.get()->points.end(), p.get()[0]); }
        void points(const double*& p)                        { std::fill(m_points.get()->points.begin(), m_points.get()->points.end(), *p); }
        void points(const std::vector<double>& p)            { this->m_points.get()->points = p; this->number(p.size()); }
        
        /**
         * Set the Colors
         */
        void colors(const std::shared_ptr<unsigned char>& c) { m_points.get()->colors = std::vector<unsigned char>(*c.get(), *(c.get() + this->number() * 3)); }
        //void colors(const std::shared_ptr<unsigned char> c)  { std::fill(m_points.get()->colors.begin(), m_points.get()->colors.end(), c.get()[0]); }
        void colors(const unsigned char*& c)                 { std::fill(m_points.get()->colors.begin(), m_points.get()->colors.end(), *c); }
        void colors(const std::vector<unsigned char>& c)     { if(c.size() == 3 * this->number()) this->m_points.get()->colors = c; } // else?
        
        //this arrays are for the points and the colors
        #pragma db type("BLOB")
        std::shared_ptr<Points> m_points;
        //Points* m_points;
        
        RDFPropertyMap::Ptr prop_;
    };
    
    // enable history:
    typedef storage::History<PointCloud::Ptr> PointCloudHistory;
    #pragma db value(PointCloudHistory)
    
}}

#endif /* end of include guard: SEMPR_ENTITY_POINTCLOUD_HPP_ */
