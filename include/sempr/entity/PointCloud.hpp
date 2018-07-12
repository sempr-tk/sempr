#ifndef SEMPR_ENTITY_POINTCLOUD_HPP_
#define SEMPR_ENTITY_POINTCLOUD_HPP_

#include <sempr/storage/History.hpp>
#include <sempr/entity/Entity.hpp>
#include <sempr/entity/RDFPropertyMap.hpp>
#include <memory>
#include <vector>
#include <algorithm>

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
         * @brief nested struct to store points in vectors
         */
        /*
        struct Points
        {
            std::vector<double> points;
            std::vector<unsigned char> colors;
        };
        */

        struct Point
        {
            double x, y, z;
            unsigned char r, g, b;
            // more for hyperspectral
        };

        #pragma db value
        struct Bounds
        {
            double minX, maxX;
            double minY, maxY;
            double minZ, maxZ;
        };

        
        uint64_t size() const { return m_points.size(); }
        Bounds bounds() const { return m_bounds; }
        
        std::vector<Point> points() { return m_points; }
        
        void setPointsWithColor(const std::vector<double> &points, const std::vector<unsigned char> &colors, uint64_t n);
        void setPoints(std::vector<Point> points) { m_points = points; changed(); }

        void addPoint(Point p) { m_points.push_back(p); changed(); }
        void removePoint(uint64_t i) { m_points.erase(m_points.begin() + (i - 1)); changed(); }

        /*
        // return single point i
        int p(uint64_t i, std::vector<double>& p);
        // single value of point
        int x(uint64_t i, double& x) { return  w(i, x, 0); }
        int y(uint64_t i, double& y) { return  w(i, y, 1); }
        int z(uint64_t i, double& z) { return  w(i, z, 2); }
        int w(uint64_t i, double& w, uint64_t j);
        
        // return single color of point i
        int c(uint64_t i, std::vector<unsigned char>& c);
        // single color
        int r(uint64_t i, unsigned char& r) { return  a(i, r, 0); }
        int g(uint64_t i, unsigned char& g) { return  a(i, g, 1); }
        int b(uint64_t i, unsigned char& b) { return  a(i, b, 2); }
        int a(uint64_t i, unsigned char& a, uint64_t j);
        */
        double x(int i) { return m_points[i].x; }
        double y(int i) { return m_points[i].y; }
        double z(int i) { return m_points[i].z; }
        unsigned char r(int i) { return m_points[i].r; }
        unsigned char g(int i) { return m_points[i].g; }
        unsigned char b(int i) { return m_points[i].b; }

        Point point(int i) { return m_points[i]; }

        virtual ~PointCloud() {}
        
    protected:
        friend class odb::access;
        
        // we should not be able to set the bounds manually - let sempr calculate them TODO
        void bounds(std::vector<double> b);

        void calculateBounds();
        
        // how can I make this save?
        /**
         * Set the Points
         */
        //void points(const std::vector<double>& p) { this->m_points.get()->points = p; this->size(p.size()); }
        
        /**
         * Set the Colors
         */
        //void colors(const std::vector<unsigned char>& c) { if(c.size() == 3 * this->size()) this->m_points.get()->colors = c; } // else?
        
        #pragma db type("BLOB")
        // @Blob:   The danger of this kind of operation is that the binary representation
        //          of the resulting array will not be portable (e.g., one applications is
        //          big-endian while another is little-endian; and in case of doubles, even
        //          ABIs with the same endian-ness can have different representations)
        // From:    https://www.codesynthesis.com/pipermail/odb-users/2012-January/000441.html
        //std::shared_ptr<Points> m_points;
        std::vector<Point> m_points;
        Bounds m_bounds;
        
        RDFPropertyMap::Ptr prop_;
    };
    
    // enable history:
    typedef storage::History<PointCloud::Ptr> PointCloudHistory;
    #pragma db value(PointCloudHistory)
    
}}

#endif /* end of include guard: SEMPR_ENTITY_POINTCLOUD_HPP_ */
