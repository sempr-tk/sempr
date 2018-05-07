#include <sempr/entity/spatial/Geometry.hpp>
#include <sempr/entity/spatial/GlobalCS.hpp>
#include <sempr/entity/spatial/LocalTransformation.hpp>
#include <Geometry_odb.h>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(Geometry)

Geometry::Geometry() : Geometry(new core::IDGen<Geometry>())
{
}

Geometry::Geometry(const core::IDGenBase* idgen)
    : Entity(idgen)
{
    this->setDiscriminator<Geometry>();
}

Geometry::~Geometry()
{
}

Geometry::Ptr Geometry::clone() const {
    // raw clone is virtual! :)
    return Geometry::Ptr(raw_clone());
}

Geometry* Geometry::raw_clone() const {
    return NULL;
}

void Geometry::setCS(SpatialReference::Ptr cs)
{
    referenceFrame_ = cs;
}

SpatialReference::Ptr Geometry::getCS() const
{
    return referenceFrame_;
}

void Geometry::transformToCS(SpatialReference::Ptr cs)
{
    if (!this->geometry()) throw TransformException("no geometry to transform");
    if (!this->referenceFrame_) throw TransformException("source reference frame invalid");
    if (!cs) throw TransformException("target reference frame invalid");

    auto rootThis = referenceFrame_->getRoot();
    auto rootOther = cs->getRoot();

    // two cases to consider:
    // 1. both geometries are in the same "tree", have the same root
    // (e.g. UTM 32N, or a common LocalCS)
    if (rootThis == rootOther) {
        // same root reference frame, can focus on LocalCS only
        auto fromThis = referenceFrame_->transformationToRoot();
        auto toOther = cs->transformationFromRoot();
        // geometry = toOther * fromThis * geometry
        LocalTransformation tf(toOther * fromThis);
        this->geometry()->transform(&tf);
    }
    // 2. the geometries have different roots (e.g., one is on WGS84,
    //    and the other in a local coordinate system relative to UTM 32N)
    else {
        GlobalCS::Ptr globalThis = std::dynamic_pointer_cast<GlobalCS>(rootThis);
        GlobalCS::Ptr globalOther = std::dynamic_pointer_cast<GlobalCS>(rootOther);

        if (!globalThis || !globalOther)
        {
            // we have two different roots that are not both of type
            // GlobalCS? no way to transform between them.
            std::cout << globalThis.get() << '\n';
            std::cout << globalOther.get() << '\n';
            throw TransformException("different root systems that are not global");
        }

        // both are global!
        auto transform = globalThis->to(globalOther);
        if (!transform) {
            // transformation unknown to GDAL / proj4?!
            throw TransformException("transform unknown to GDAL/proj4");
        }
        // 3 steps:
        // 1: this from ref to this->getRoot()
        auto thisToRoot = referenceFrame_->transformationToRoot();
        LocalTransformation tfToRoot(thisToRoot);
        this->geometry()->transform(&tfToRoot);
        // 2: from this->getRoot() to cs->getRoot()
        this->geometry()->transform(transform.get());
        // 3: from cs->getRoot() to cs
        auto rootToCS = cs->transformationFromRoot();
        LocalTransformation tfToCS(rootToCS);
        this->geometry()->transform(&tfToCS);
    }

    // set new reference frame
    referenceFrame_ = cs;
}


}}
