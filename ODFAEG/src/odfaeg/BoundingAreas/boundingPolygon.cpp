#include "../../../include/odfaeg/BoundingAreas/boundingPolygon.h"
#include "../../../include/odfaeg/BoundingAreas/boundingCircle.h"
#include "../../../include/odfaeg/BoundingAreas/boundingEllipse.h"
#include "../../../include/odfaeg/BoundingAreas/boundingRectangle.h"
#include "../../../include/odfaeg/BoundingAreas/orientedBoundingRect.h"
namespace odfaeg {
BoundingPolyhedron::BoundingPolyhedron () {
}
void BoundingPolyhedron::addPoint(Vec3f *point) {
    points.push_back(point);
    computeCenter();
}
bool BoundingPolyhedron::intersects (BoundingSphere &bs) {
    return bs.intersects (*this);
}
bool BoundingPolyhedron::intersects (BoundingEllipsoid &be) {
    return be.intersects (*this);
}
bool BoundingPolyhedron::intersects (BoundingBox& bx) {
    return bx.intersects (*this);
}
bool BoundingPolyhedron::intersects (OrientedBoundingBox &obx) {
    return obx.intersects (*this);
}
bool BoundingPolyhedron::intersects (BoundingPolyhedron &bp) {
     Vec3f d = bp.getCenter() - center;
     float m = center.computeDist(bp.getCenter());
     Vec3f a1 = *points[0];
     Vec3f b1 = *points[1];
     Vec3f mid1 = a1 + (b1 - a1) * 0.5f;
     Vec3f bi1 = mid1 - center;
     float proj1 = d.projOnAxis(bi1);
     int min1 = proj1;
     int max1 = proj1;
     int ray1;
     for (unsigned int j = 1; j < points.size(); j++) {
        a1 = *points[j];
        if (j != points.size() - 1)
            b1 = *points[j+1];
        else
            b1 = *points[0];
        mid1 = a1 + (b1 - a1) * 0.5f;
        bi1 = (mid1 - center) * 0.5f;
        proj1 = d.projOnAxis(bi1);
        if (proj1 < min1)
            min1 = proj1;
        if (proj1 > max1)
            max1 = proj1;
    }
    ray1 = max1 - min1;
    Vec3f a2 = bp.getPoint(0);
    Vec3f b2 = bp.getPoint(1);
    Vec3f mid2 = a2 + (b2 - a2) * 0.5f;
    Vec3f bi2 = mid2 - bp.getCenter();
    float proj2 = d.projOnAxis(bi2);
    int min2 = proj2;
    int max2 = proj2;
    int ray2;
    for (unsigned int j = 1; j < bp.nbPoints(); j++) {
       a2 = bp.getPoint(j);
       if (j != bp.nbPoints() - 1)
           b2 = bp.getPoint(j+1);
       else
           b2 = bp.getPoint(0);
       mid2 = a2 + (b2 - a2) * 0.5f;
       bi2 = (mid2 - bp.getCenter()) * 0.5f;
       proj2 = d.projOnAxis(bi2);
       if (proj2 < min2)
           min2 = proj2;
       if (proj2 > max2)
           max2 = proj2;
    }
    ray2 = max2 - min2;
    int rSum = ray1 + ray2;
    if (m - rSum > 0)
        return false;
    return true;
}
bool BoundingPolyhedron::isPointInside(Vec3f &p) {
    Vec3f d = p - center;
    float m = center.computeDist(p);
    Vec3f a1 = *points[0];
    Vec3f b1 = *points[1];
    Vec3f mid1 = a1 + (b1 - a1) * 0.5f;
    Vec3f bi1 = mid1 - center;
    float proj1 = d.projOnAxis(bi1);
    int min1 = proj1;
    int max1 = proj1;
    int ray1;
    for (unsigned int j = 1; j < points.size(); j++) {
        a1 = *points[j];
        if (j != points.size() - 1)
            b1 = *points[j+1];
        else
            b1 = *points[0];
        mid1 = a1 + (b1 - a1) * 0.5f;
        bi1 = (mid1 - center) * 0.5f;
        proj1 = d.projOnAxis(bi1);
        if (proj1 < min1)
            min1 = proj1;
        if (proj1 > max1)
            max1 = proj1;
   }
   int ray2 = m;
   int rSum = ray1 + ray2;
   if (m - rSum > 0)
       return false;
   return true;
}
int BoundingPolyhedron::nbPoints () {
    return points.size();
}
void BoundingPolyhedron::computeCenter () {
    Vec3f sum(0.f, 0.f, 0.f);
    for (unsigned int i = 0; i < points.size(); i++) {
        sum += *points[i];
    }
    center = sum / points.size();

}
BoundingPolyhedron::~BoundingPolyhedron () {
    for (unsigned int i = 0; i < points.size(); i++) {
        delete points[i];

    }
    points.clear();

}
bool BoundingPolyhedron::operator== (const BoundingPolyhedron &bp) {
    if(points.size() != bp.points.size())
        return false;
    for (unsigned int i = 0; i < points.size(); i++) {
        if (!(*points[i] == *bp.points[i])) {
            return false;
        }
    }
    return true;
}
Vec3f BoundingPolyhedron::getPoint(unsigned  int index) {
    if (index >= 0 && index < points.size())
        return *points[index];
    else
        return Vec3f (0, 0, 0);
}
Vec3f BoundingPolyhedron::getCenter () {
    return center;
}
}


