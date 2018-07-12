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
         * @brief nested struct to store points in vectors
         */
        struct Points
        {
            std::vector<double> points;
            std::vector<unsigned char> colors;
        };
        
        uint64_t size() const { return (uint64_t)((*prop_)["size"]); }
        bool color() const { return (bool)((*prop_)["color"]); }
        std::vector<double> bounds() const;
        
        std::vector<double> points() { return m_points.get()->points; }
        
        std::vector<unsigned char> colors() { return m_points.get()->colors; }
        
        void setPointsWithColor(std::vector<double>& points, std::vector<unsigned char>& colors, uint64_t n);
        
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

        virtual ~PointCloud() {}
        
    protected:
        friend class odb::access;
        
        // we should not be able to set the bounds manually - let sempr calculate them TODO
        void bounds(std::vector<double> b);

        void size(uint64_t n) { (*prop_)["size"] = (qulonglong)n; changed(); prop_->changed(); }
        void color(bool c) { (*prop_)["color"] = (bool)c; changed(); prop_->changed(); }
        
        // how can I make this save?
        /**
         * Set the Points
         */
        void points(const std::vector<double>& p) { this->m_points.get()->points = p; this->size(p.size()); }
        
        /**
         * Set the Colors
         */
        void colors(const std::vector<unsigned char>& c) { if(c.size() == 3 * this->size()) this->m_points.get()->colors = c; } // else?
        
        #pragma db type("BLOB")
        std::shared_ptr<Points> m_points;
        
        RDFPropertyMap::Ptr prop_;
    };
    
    // enable history:
    typedef storage::History<PointCloud::Ptr> PointCloudHistory;
    #pragma db value(PointCloudHistory)
    
}}

#endif /* end of include guard: SEMPR_ENTITY_POINTCLOUD_HPP_ */
