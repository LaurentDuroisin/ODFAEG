#include "../../../include/odfaeg/BoundingAreas/boundingCircle.h"
#include "../../../include/odfaeg/BoundingAreas/boundingEllipse.h"
#include "../../../include/odfaeg/BoundingAreas/boundingRectangle.h"
#include "../../../include/odfaeg/BoundingAreas/orientedBoundingRect.h"
#include "../../../include/odfaeg/BoundingAreas/boundingPolygon.h"
namespace odfaeg {
/*Crée un cercle englobant, utilisé pour les détection de collisions, et la sélection
* d'objets.
*/
//Crée un cercle de centre center et de rayon radius.
BoundingSphere::BoundingSphere (Vec3f center, float radius) {
    this->center = center;
    this->radius = radius;
}
//Test si un cercle en intersecte un autre.
bool BoundingSphere::intersects (BoundingSphere &other) {
    float d = center.computeDist (other.center);
    float rSum = radius + other.radius;
    return (d <= rSum);
}
bool BoundingSphere::intersects (BoundingEllipsoid &be) {
    return be.intersects(*this);
}
//Test si un rectangle intersecte un cercle. (Théorême de l'axe séparateur.)
bool BoundingSphere::intersects (BoundingBox &bx) {
    Vec3f d = bx.getCenter() - center;
    float m = center.computeDist(bx.getCenter());
    Vec3f near, far;
    Ray r (bx.getCenter(), center);
    intersectsWhere(r, near, far);
    Vec3f d2 = near - center;
    int ray2 = d.projOnAxis(d2);
    float hi[3];
    hi[0] = bx.getWidth() * 0.5f;
    hi[1] = bx.getHeight() * 0.5f;
    hi[2] = bx.getHeight() * 0.5f;
    for (int i = 0; i < 3; i++) {
        int rSum = radius + hi[i];
        if (m - rSum > 0.f)
            return false;
    }
    return true;
}

bool BoundingSphere::intersects (OrientedBoundingBox &obx) {
    Vec3f d = obx.getCenter() - center;
    float m = center.computeDist(obx.getCenter());
    Vec3f near, far;
    Ray r(obx.getCenter(), center);
    intersectsWhere(r, near, far);
    Vec3f d2 = near - center;
    int ray2 = d.projOnAxis(d2);
    Vec3f bi[3];
    bi[0] = obx.getBX() * obx.getWidth() * 0.5f;
    bi[1] = obx.getBY() * obx.getHeight() * 0.5f;
    bi[2] = obx.getBZ() * obx.getDepth() * 0.5f;
    for (int i = 0; i < 3; i++) {
        float ray2 = d.projOnAxis(bi[i]);
        int rSum = radius + ray2;
        if (m - rSum > 0.f)
            return true;
    }
    return false;
}
bool BoundingSphere::intersects (BoundingPolyhedron &bp) {
    Vec3f d = bp.getCenter() - center;
    float m = center.computeDist(bp.getCenter());
    Vec3f near, far;
    Ray r(bp.getCenter(), center);
    intersectsWhere(r, near, far);
    Vec3f d2 = near - center;
    int ray2 = d.projOnAxis(d2);
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
    ray2 = max - min;
    int rSum = radius + ray2;
    if (m - rSum > 0.f)
        return true;
    return false;
}
bool BoundingSphere::intersects (Ray &r) {
    Vec3f d = r.getOrig() - center;
    float a = r.getDir().dot(r.getDir());
    float b = 2 * r.getDir().dot(d);
    float c = d.dot(d) - (radius * radius);
    float disc = b * b - 4 *  a * c;
    if (disc < 0)
        return false;
    float distSqrt = Math::sqrt(disc);
    float q;
    if (b < 0)
        q = (-b - distSqrt)/2.0;
    else
        q = (-b + distSqrt)/2.0;

    // compute t0 and t1
    float t0 = q / a;
    float t1 = c / q;

    // make sure t0 is smaller than t1
    if (t0 > t1)
    {
        // if t0 is bigger than t1 swap them around
        float temp = t0;
        t0 = t1;
        t1 = temp;
    }

    // if t1 is less than zero, the object is in the ray's negative direction
    // and consequently the ray misses the sphere
    if (t1 < 0)
        return false;

    // if t0 is less than zero, the intersection point is at t1
    if (t0 < 0)
    {
        return true;
    }
    // else the intersection point is at t0
    else
    {
        return true;
    }
}
bool BoundingSphere::intersectsWhere (Ray& r, Vec3f& near, Vec3f &far) {
    Vec3f d = r.getOrig() - center;
    float a = r.getDir().dot(r.getDir());
    float b = 2 * r.getDir().dot(d);
    float c = d.dot(d) - (radius * radius);
    float disc = b * b - 4 *  a * c;
    if (disc < 0)
        return false;
    float distSqrt = Math::sqrt(disc);
    float q;
    if (b < 0)
        q = (-b - distSqrt)/2.0;
    else
        q = (-b + distSqrt)/2.0;

    // compute t0 and t1
    float t0 = q / a;
    float t1 = c / q;

    // make sure t0 is smaller than t1
    if (t0 > t1)
    {
        // if t0 is bigger than t1 swap them around
        float temp = t0;
        t0 = t1;
        t1 = temp;
    }

    // if t1 is less than zero, the object is in the ray's negative direction
    // and consequently the ray misses the sphere
    if (t1 < 0)
        return false;

    // if t0 is less than zero, the intersection point is at t1
    bool intersects = false;
    if (t0 < 0)
    {
        near = r.getOrig() + r.getDir() * t1;
        intersects = true;
    }
    // else the intersection point is at t0
    else
    {
        far = r.getOrig() + r.getDir() * t0;
        if (!intersects)
            intersects = true;
    }
    return intersects;
}
//Test si un point est à l'intérieur du cercle.
bool BoundingSphere::isPointInside (Vec3f &point) {
    float d = center.computeDist(point);
    return d < radius;
}
Vec3f BoundingSphere::getCenter() {
    return center;
}
float BoundingSphere::getRadius () {
    return radius;
}
}

