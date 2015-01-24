#include "../../../include/odfaeg/BoundingAreas/boundingEllipse.h"
#include "../../../include/odfaeg/BoundingAreas/boundingCircle.h"
#include "../../../include/odfaeg/BoundingAreas/boundingRectangle.h"
#include "../../../include/odfaeg/BoundingAreas/orientedBoundingRect.h"
#include "../../../include/odfaeg/BoundingAreas/boundingPolygon.h"
#include "../../../include/odfaeg/math/Matrix4.h"
namespace odfaeg {
BoundingEllipsoid::BoundingEllipsoid(Vec3f center, int a, int b, int c) {
    this->center = center;
    radius = Vec3f(a, b, c);
}
bool BoundingEllipsoid::intersects(BoundingSphere &bs) {
    //We do the same for check the collision with a circle, except taht the circle have only 1 ray.
    //The equation for the circle is then a bit different.
    Vec3f d = bs.getCenter() - center;
    float m = center.computeDist(bs.getCenter());
    Vec3f near, far;
    Ray r(bs.getCenter(), center);
    intersectsWhere(r, near, far);
    Vec3f d2 = near - center;
    int ray1 = d.projOnAxis(d2);
    d = center - bs.getCenter();
    bs.intersectsWhere(r,near,far);
    d2 = near - bs.getCenter();
    int ray2 = d.projOnAxis(d2);
    float rSum = ray1 + ray2;
    if (m - rSum > 0)
        return false;
    return true;
}
bool BoundingEllipsoid::intersects(BoundingEllipsoid &be) {
     //The distance between the 2 centers.
    Vec3f d = be.getCenter() - center;
    float m = center.computeDist(be.getCenter());
    Vec3f near, far;
    Ray r(be.getCenter(), center);
    intersectsWhere(r, near, far);
    Vec3f d2 = near - center;
    int ray1 = d.projOnAxis(d2);
    d = center - be.getCenter();
    r = Ray(center, be.getCenter());
    be.intersectsWhere(r,near,far);
    d2 = near - be.getCenter();
    int ray2 = d.projOnAxis(d2);
    float rSum = ray1 + ray2;
    if (m - rSum > 0)
        return false;
    return true;
}
bool BoundingEllipsoid::intersects(BoundingBox &bx) {
    Vec3f d = bx.getCenter() - center;
    float m = center.computeDist(bx.getCenter());
    Vec3f near, far;
    Ray r(bx.getCenter(), center);
    intersectsWhere(r, near, far);
    Vec3f d2 = near - center;
    int ray = d.projOnAxis(d2);
    float hi[3];
    hi[0] = bx.getWidth() * 0.5f;
    hi[1] = bx.getHeight() * 0.5f;
    hi[2] = bx.getDepth() * 0.5f;
    for (unsigned int i = 0; i < 3; i++) {
        //Do the sum of the 2 rays of our bounding volumes.
        float rSum = ray + hi[i];
        /*If the distance between the two centers is shorter than the sum of the rays,
         * it means that our two bouding volumes are in collision.
         *(Separator axis theorem)
         */
         if (m - rSum > 0)
            return false;
    }
    return true;
}
bool BoundingEllipsoid::intersects(OrientedBoundingBox &obx) {
    //The direction of the straigth between the two centers.
    Vec3f d = obx.getCenter() - center;
    float m = center.computeDist(obx.getCenter());
    Vec3f near, far;
    Ray r(obx.getCenter(), center);
    intersectsWhere(r, near, far);
    Vec3f d2 = near - center;
    int ray1 = d.projOnAxis(d2);
    Vec3f bi[3];
    //Vectors between the center of the oriented bounding rectangle, and the halfs of the edges.
    bi[0] = obx.getBX() * obx.getWidth() * 0.5f;
    bi[1] = obx.getBY() * obx.getHeight() * 0.5f;
    bi[2] = obx.getBZ() * obx.getDepth() * 0.5f;
    for (unsigned int i = 0; i < 3; i++) {
        //Projection of the ray of our oriented bouding rect from d to the x and y axis.
        float ray2 = d.projOnAxis(bi[i]);
        //Do the sum of the 2 rays of our bounding volumes.
        float rSum = ray1 + ray2;
        /*If the distance between the two centers is shorter than the sum of the rays,
         * it means that our two bouding volumes are in collision.
         *(Separator axis theorem)
         */
         if (m - rSum > 0)
            return false;
    }
    return true;
}
bool BoundingEllipsoid::intersects(BoundingPolyhedron &bp) {
    //The direction of the straigth between the two centers.
    Vec3f d = bp.getCenter() - center;
    //The distance between the 2 centers.
    float m = center.computeDist(bp.getCenter());
    Vec3f near, far;
    Ray r(bp.getCenter(), center);
    intersectsWhere(r, near, far);
    Vec3f d2 = near - center;
    int ray1 = d.projOnAxis(d2);
    Vec3f a = bp.getPoint(0);
    Vec3f b = bp.getPoint(1);
    Vec3f mid = a + (b - a) * 0.5f;
    Vec3f bi = mid - bp.getCenter();
    float proj = d.projOnAxis(bi);
    int min = proj;
    int max = proj;
    for (unsigned int j = 1; j < bp.nbPoints(); j++) {
       a = bp.getPoint(j);
       if (j != bp.nbPoints() - 1)
           b = bp.getPoint(j+1);
       else
           b = bp.getPoint(0);
       mid = a + (b - a) * 0.5f;
       bi = (mid - bp.getCenter()) * 0.5f;
       proj = d.projOnAxis(bi);
       if (proj < min)
           min = proj;
       if (proj > max)
           max = proj;
    }
    int ray2 = max - min;
    int rSum = ray1 + ray2;
    if (m - rSum > 0.f)
        return false;
    return false;
}
bool BoundingEllipsoid::intersects (Ray &r) {
    Matrix4f scale (radius.x, 0, 0, 0,
                    0, radius.y, 0, 0,
                    0, 0, radius.z, 0,
                    0, 0, 0, 1);
    Matrix4f rotation (1, 0, 0, 0,
                       0, 1, 0, 0,
                       0, 0, 1, 0,
                       0, 0, 0, 1);
    Matrix4f translation (1, 0, 0, center.x,
                        0, 1, 0, center.y,
                        0, 0, 1, center.z,
                        0, 0, 0, 1);
    Matrix4f transform = scale * rotation * translation;
    Matrix4f invTransform = transform.inverse();
    Vec3f orig = invTransform * r.getOrig();
    Vec3f ext = invTransform * r.getOrig();
    Ray tRay (orig, ext);
    for (unsigned int i = 0; i < 3; i++) {
        BoundingSphere bs(Vec3f(0, 0, 0), radius[i]);
        if (!bs.intersects(tRay))
            return false;
    }
    return true;
}
bool BoundingEllipsoid::intersectsWhere(Ray& r, Vec3f& near, Vec3f& far) {
    Matrix4f scale (radius.x, 0, 0, 0,
                    0, radius.y, 0, 0,
                    0, 0, radius.z, 0,
                    0, 0, 0, 1);
    Matrix4f rotation (1, 0, 0, 0,
                       0, 1, 0, 0,
                       0, 0, 1, 0,
                       0, 0, 0, 1);
    Matrix4f translation (1, 0, 0, center.x,
                        0, 1, 0, center.y,
                        0, 0, 1, center.z,
                        0, 0, 0, 1);
    Matrix4f transform = scale * rotation * translation;
    Matrix4f invTransform = transform.inverse();
    Vec3f orig = invTransform * r.getOrig();
    Vec3f ext = invTransform * r.getOrig();
    Ray tRay (orig, ext);
    Vec3f i1, i2;
    for (unsigned int i = 0; i < 3; i++) {
        BoundingSphere bs(Vec3f(0, 0, 0), radius[i]);
        Vec3f tmpi1, tmpi2;
        if (!bs.intersectsWhere(tRay, tmpi1, tmpi2))
            return false;
        i1[i] = tmpi1[i];
        i2[i] = tmpi2[i];
    }
    near = transform * i1;
    far = transform * i2;
    return true;
}
bool BoundingEllipsoid::isPointInside (Vec3f point) {
    Ray r (center, point);
    if (!intersects(r))
        return false;
    return true;
}
Vec3f BoundingEllipsoid::getCenter() {
    return center;
}
Vec3f BoundingEllipsoid::getRadius() {
    return radius;
}
}
