#include "../../../include/odfaeg/BoundingAreas/boundingRectangle.h"
#include "../../../include/odfaeg/BoundingAreas/boundingCircle.h"
#include "../../../include/odfaeg/BoundingAreas/boundingEllipse.h"
#include "../../../include/odfaeg/BoundingAreas/orientedBoundingRect.h"
#include "../../../include/odfaeg/BoundingAreas/boundingPolygon.h"
using namespace std;
namespace odfaeg {

/*Crée un rectangle englobant pour la détection de collision et la sélection
*d'objets.
*/
BoundingBox::BoundingBox() {
    int x = y = z= width = height = depth = 0;
}
//Crée un rectangle de largeur width, de hauteur height et à la position (x, y)
BoundingBox::BoundingBox (int x, int y, int z, int width, int height, int depth) :  center (x + width * 0.5f, y + height * 0.5f, z + depth * 0.5f) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->width = width;
    this->height = height;
    this->depth = depth;

}
//Test si le rectangle intersecte un cercle.
bool BoundingBox::intersects (BoundingSphere &bs) {
    return bs.intersects(*this);
}
bool BoundingBox::intersects (BoundingEllipsoid &be) {
    return be.intersects(*this);
}
//Test si un rectangle en intersecte un autre.
bool BoundingBox::intersects (BoundingBox &other) {
    int hx1 = width * 0.5;
    int hy1 = height * 0.5;
    int hz1 = depth * 0.5;
    int hx2 = other.width * 0.5;
    int hy2 = other.height * 0.5;
    int hz2 = other.depth * 0.5;
    float minX1 = center.x - hx1, minX2 = other.center.x - hx2;
    float minY1 = center.y - hy1, minY2 = other.center.y - hy2;
    float minZ1 = center.z - hz1, minZ2 = other.center.z - hz2;
    float maxX1 = center.x + hx1, maxX2 = other.center.x + hx2;
    float maxY1 = center.y + hy1, maxY2 = other.center.y + hy2;
    float maxZ1 = center.z + hz1, maxZ2 = other.center.z + hz2;
    for (int i = 0; i < 3; i++) {
        if (i == 0) {
            if (minX1 > maxX2 || maxX1 < minX2)
                return false;
        } else if (i == 1) {
            if (minY1 > maxY2 || maxY1 < minY2)
                return false;
        } else {
            if (minZ1 > maxZ2 || maxZ1 < minZ2)
                return false;
        }
    }
    return true;
}
bool BoundingBox::intersects(OrientedBoundingBox &obx) {
    return obx.intersects(*this);
}
bool BoundingBox::intersects (BoundingPolyhedron &bp) {
     Vec3f d = bp.getCenter() - center;
     float m = center.computeDist(bp.getCenter());
     float hi[3];
     hi[0] = width * 0.5f;
     hi[1] = height * 0.5f;
     hi[2] = depth * 0.5f;
     for (unsigned int i = 0; i < 3; i++) {
         Vec3f a = bp.getPoint(0);
         Vec3f b = bp.getPoint(1);
         Vec3f mid = a + (b - a) * 0.5f;
         Vec3f bi = mid - bp.getCenter();
         float proj = d.projOnAxis(bi);
         int min = proj;
         int max = proj;
         int ray;
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
        ray = max - min;
        int rSum = hi[i] + ray;
        if (m - rSum > 0.f)
            return false;
     }
     return true;
}
//Test si un point est dans le rectangle.
bool BoundingBox::isPointInside (Vec3f point) {
    float minX = center.x - width * 0.5f;
    float maxX = center.x + width * 0.5f;
    float minY = center.y - height * 0.5f;
    float maxY = center.y + height * 0.5f;
    float minZ = center.z - depth * 0.5f;
    float maxZ = center.z + depth * 0.5f;

    return (point.x >= minX && point.x <= maxX && point.y >= minY && point.y <= maxY && point.z >= minZ && point.z <= maxZ);
}

bool BoundingBox::intersects (Ray &ray) {
    float tFar = INT_MAX;
    float tNear = -INT_MAX;

    for (int i = 0; i < 3; i++) {
        float d, orig, hi;
        if (i == 0) {
            hi = width / 2;
            d = ray.getDir().x;
            orig = ray.getOrig().x - center.x;
        } else if (i == 1) {
            hi = height / 2;
            d = ray.getDir().y;
            orig = ray.getOrig().y - center.y;
        } else {
            hi = depth / 2;
            d = ray.getDir().z;
            orig = ray.getOrig().z - center.z;
        }

        if (d == 0)
            if (Math::abs(orig) > hi)
                return false;
        float t1 = (-hi - orig) / d;
        float t2 = (hi - orig) / d;
        float tmp;
        if (t1 > t2) {
            tmp = t1;
            t1 = t2;
            t2 = tmp;
        } else
            tmp = t1;
        if (t1 > tNear)
            tNear = t1;
        if (t2 < tFar)
            tFar = t2;

        if (tNear > tFar)
            return false;

        if(tFar < 0)
            return false;
    }
    return true;

}
//Acceseurs.
Vec3f BoundingBox::getCenter () {
    return center;
}
float BoundingBox::getWidth () {
    return width;
}
float BoundingBox::getHeight () {
    return height;
}
float BoundingBox::getDepth() {
    return depth;
}
Vec3f BoundingBox::getPosition () {
    return Vec3f (x, y, z);
}
void BoundingBox::setPosition(int x, int y, int z) {
    this->x = x;
    this->y = y;
    this->z = z;
}
void BoundingBox::setSize(int width, int height, int depth) {
    this->width = width;
    this->height = height;
    this->depth = depth;
}
}
