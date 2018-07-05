#include <sempr/entity/spatial/LocalTransformation.hpp>

namespace sempr {

LocalTransformation::LocalTransformation(const Eigen::Affine3d& aff)
    : mat_(aff)
{
}

/*
OGRSpatialReference* LocalTransformation::GetSourceCS()
{
    return NULL;
}
*/

/*
OGRSpatialReference* LocalTransformation::GetTargetCS()
{
    return NULL;
}
*/

int LocalTransformation::Transform(int nCount, double *x, double *y, double *z)
{
    return TransformEx(nCount, x, y, z);
}

int LocalTransformation::TransformEx(int nCount, double *x, double *y, double *z, int *pabSuccess)
{
    // note: gdal always gives us a z-vector, too, regardless the actual type of point.
    // but it works nevertheless: no measurements are overriden, not z-coordinate set by accident,
    // the interpretation of the point stays valid.
    if (z) {
        tf3D(nCount, x, y, z, pabSuccess);
    } else {
        tf2D(nCount, x, y, pabSuccess);
    }
    return true;
}

void LocalTransformation::tf2D(int nCount, double *x, double *y, int *pabSuccess)
{
    /**
        Affine3d:
            0 1 2    3
        0 ( a b c | tx )
        1 ( d e f | ty )
        2 ( g h i | tz )
        (-------+----)
        3 ( 0 0 0 |  1 )

        In the 2D case, we only need a small portion of it:
        ( a b | tx )
        ( d e | ty )
        because:
        x' = a*x + b*y + tx
        y' = d*x + e*y + ty
    */
    double a, b, d, e, tx, ty;
    a = mat_.matrix().coeff(0, 0);
    b = mat_.matrix().coeff(0, 1);
    d = mat_.matrix().coeff(1, 0);
    e = mat_.matrix().coeff(1, 1);
    tx = mat_.matrix().coeff(0, 3);
    ty = mat_.matrix().coeff(1, 3);

    // #pragma omp parallel for
    for (int i = 0; i < nCount; i++)
    {
        double tmpX = x[i];
        double tmpY = y[i];
        x[i] = a*tmpX + b*tmpY + tx;
        y[i] = d*tmpX + e*tmpY + ty;
        if (pabSuccess) pabSuccess[i] = true;
    }
}


void LocalTransformation::tf3D(int nCount, double *x, double *y, double *z, int *pabSuccess)
{
    /**
        Affine3d:
        ( a b c | tx )
        ( d e f | ty )
        ( g h i | tz )
        (-------+----)
        ( 0 0 0 |  1 )

        x' = a*x + b*y + c*z + tx
        y' = d*x + e*y + f*z + ty
        z' = g*x + h*y + i*z + tz
    */
    double a, b, c, d, e, f, g, h, i, tx, ty, tz;
    a = mat_.matrix().coeff(0, 0);
    b = mat_.matrix().coeff(0, 1);
    c = mat_.matrix().coeff(0, 2);

    d = mat_.matrix().coeff(1, 0);
    e = mat_.matrix().coeff(1, 1);
    f = mat_.matrix().coeff(1, 2);

    g = mat_.matrix().coeff(2, 0);
    h = mat_.matrix().coeff(2, 1);
    i = mat_.matrix().coeff(2, 2);

    tx = mat_.matrix().coeff(0, 3);
    ty = mat_.matrix().coeff(1, 3);
    tz = mat_.matrix().coeff(2, 3);

    // TODO: Why does omp make this slow, even for large number of points?
    // #pragma omp parallel for
    for (int n = 0; n < nCount; n++)
    {
        double tmpX = x[n];
        double tmpY = y[n];
        double tmpZ = z[n];
        x[n] = a*tmpX + b*tmpY + c*tmpZ + tx;
        y[n] = d*tmpX + e*tmpY + f*tmpZ + ty;
        z[n] = g*tmpX + h*tmpY + i*tmpZ + tz;
        if (pabSuccess) pabSuccess[n] = true;
    }
}


} /* sempr */
