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
    double b[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    m["xmin"] = b[0];
    m["xmax"] = b[1];
    m["ymin"] = b[2];
    m["ymax"] = b[3];
    m["zmin"] = b[4];
    m["zmax"] = b[5];

    uint64_t n = 0;
    m["number"] = (qulonglong)n;

    m["color"] = true;

    m("type", core::rdf::baseURI()) = RDFResource("<" + sempr::baseURI() + "PointCloud" + ">");
}

std::shared_ptr<double> PointCloud::bounds() const
{
    RDFPropertyMap& m = *prop_;

    std::shared_ptr<double> b(new double[6], std::default_delete<double[]>());

    b.get()[0] = m["xmin"];
    b.get()[1] = m["xmax"];
    b.get()[2] = m["ymin"];
    b.get()[3] = m["ymax"];
    b.get()[4] = m["zmin"];
    b.get()[5] = m["zmax"];

    return b;
}

void PointCloud::bounds(double *b)
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

void PointCloud::bounds(std::shared_ptr<double> b)
{
    RDFPropertyMap& m = *prop_;

    m["xmin"] = b.get()[0];
    m["xmax"] = b.get()[1];
    m["ymin"] = b.get()[2];
    m["ymax"] = b.get()[3];
    m["zmin"] = b.get()[4];
    m["zmax"] = b.get()[5];
    changed();
    prop_->changed();
}

void PointCloud::setPointsWithColor(std::shared_ptr<double>& points,
                                    std::shared_ptr<unsigned char>& colors,
                                    uint64_t n)
{    
    if(!this->m_points)
    {
        this->m_points = PointCloud::Points::Ptr(new PointCloud::Points(n, this));
    }

    // we need to trust the input? :(
    // maybe think about some saver method to get the points (and the number)
    this->points(points);
    this->colors(colors);
    this->color(0);
}

}}