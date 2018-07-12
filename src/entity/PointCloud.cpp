#include <sempr/entity/PointCloud.hpp>
#include <PointCloud_odb.h>

#include <sempr/core/RDF.hpp>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(PointCloud)

PointCloud::PointCloud() : PointCloud(new sempr::core::IDGen<PointCloud>())
{

}

PointCloud::PointCloud(const sempr::core::IDGenBase* idgen) : sempr::entity::Entity(idgen)
{
    prop_ = RDFPropertyMap::Ptr(new RDFPropertyMap(*this));
    std::cout << id() << " created an RDFPropertyMap (in PC), namely " << prop_->id() << '\n';

    // Just like in the Person class
    registerChildEntity(prop_);
    
    RDFPropertyMap& m = *prop_;

    m["xmin"] = 0.0;
    m["xmax"] = 0.0;
    m["ymin"] = 0.0;
    m["ymax"] = 0.0;
    m["zmin"] = 0.0;
    m["zmax"] = 0.0;

    uint64_t n = 0;
    m["size"] = (qulonglong)n;

    m["color"] = true;

    m("type", core::rdf::baseURI()) = RDFResource("<" + sempr::baseURI() + "PointCloud" + ">");
}

std::vector<double> PointCloud::bounds() const
{
    RDFPropertyMap& m = *prop_;

    std::vector<double> b(6);

    b[0] = m["xmin"];
    b[1] = m["xmax"];
    b[2] = m["ymin"];
    b[3] = m["ymax"];
    b[4] = m["zmin"];
    b[5] = m["zmax"];

    return b;
}

int PointCloud::p(uint64_t i, std::vector<double>& p)
{
    if (p.size() != 3) // hmm ...
        p.resize(3);
    if( (this->x(i, p[0]) == 0) && (this->y(i, p[1]) == 0) && (this->z(i, p[2]) == 0))
        return 0;
    return -1;
}

int PointCloud::w(uint64_t i, double& w, uint64_t j)
{
    if (i < this->size())
    {
        w = m_points.get()->points[(i * 3) + j];
        return 0;
    }
    return -1;
}

int PointCloud::c(uint64_t i, std::vector<unsigned char>& c)
{
    if (c.size() != 3)
        c.resize(3);
    if( (this->r(i, c[0]) == 0) && (this->g(i, c[1]) == 0) && (this->b(i, c[2]) == 0))
        return 0;
    return -1;
}

int PointCloud::a(uint64_t i, unsigned char& a, uint64_t j)
{
    // check for m_colors.emtpy() ?
    // or check for i < m_colors.capacity() ?
    if (i < this->size())
    {
        a = m_points.get()->colors[(i * 3) + j];
        return 0;
    }
    return -1;
}


void PointCloud::bounds(std::vector<double> b)
{
    RDFPropertyMap& m = *prop_;

    m["xmin"] = b[0];
    m["xmax"] = b[1];
    m["ymin"] = b[2];
    m["ymax"] = b[3];
    m["zmin"] = b[4];
    m["zmax"] = b[5];
    changed();
    prop_->changed();
}

void PointCloud::setPointsWithColor(std::vector<double>& points,
                                    std::vector<unsigned char>& colors,
                                    uint64_t n)
{
    std::cout << "in" << std::endl;
    if(this->m_points == NULL)
    {
        //this->m_points = new Points();
        this->m_points = std::shared_ptr<Points>(new Points);
    }

    // we need to trust the input? :(
    // maybe think about some saver method to get the points (and the number)
    this->size(n);
    this->points(points);
    this->colors(colors);
    this->color(0);

    std::cout << "process finished" << std::endl;
    std::cout << "Testpoint: " << this->m_points->points[n/3] << std::endl;

}

}}
