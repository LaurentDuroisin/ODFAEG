#include "../../../include/odfaeg/Physics/boundingBox.h"
#include "../../../include/odfaeg/Physics/boundingSphere.h"
#include "../../../include/odfaeg/Physics/boundingEllipsoid.h"
#include "../../../include/odfaeg/Physics/orientedBoundingBox.h"
#include "../../../include/odfaeg/Physics/boundingPolyhedron.h"
#include "../../../include/odfaeg/Graphics/transformMatrix.h"
using namespace std;
namespace odfaeg {
    namespace physic {
        //Create a bounding box at position 0, 0, 0 and with the dimensions 1, 1, 1.
        BoundingBox::BoundingBox() {
            int x = y = z = 0;
            width = height = depth = 0;
            points[0] = math::Vec3f(x, y, z);
            points[1] = math::Vec3f(x + width, y, z);
            points[2] = math::Vec3f(x + width, y + height, z);
            points[3] = math::Vec3f(x, y + height, z);
            points[4] = math::Vec3f(x, y, z + depth);
            points[5] = math::Vec3f(x + width, y, z + depth);
            points[6] = math::Vec3f(x + width, y + height, z + depth);
            points[7] = math::Vec3f(x, y + height, z + depth);
        }
        //Create a bounding box with the specified , y, z position and of width, height, depth dimensions.
        BoundingBox::BoundingBox (int x, int y, int z, int width, int height, int depth) : center (x + width * 0.5f, y + height * 0.5f, z + depth * 0.5f) {
            this->x = x;
            this->y = y;
            this->z = z;
            this->width = width;
            this->height = height;
            this->depth = depth;
            points[0] = math::Vec3f(x, y, z);
            points[1] = math::Vec3f(x + width, y, z);
            points[2] = math::Vec3f(x + width, y + height, z);
            points[3] = math::Vec3f(x, y + height, z);
            points[4] = math::Vec3f(x, y, z + depth);
            points[5] = math::Vec3f(x + width, y, z + depth);
            points[6] = math::Vec3f(x + width, y + height, z + depth);
            points[7] = math::Vec3f(x, y + height, z + depth);
        }
        bool BoundingBox::intersects (BoundingVolume& bv) {
            return false;
        }
        //Test if the box intersects the specified sphere.
        bool BoundingBox::intersects (BoundingSphere &bs) {
            return bs.intersects(*this);
        }
        //Test if the box intersects the specified ellipsoid.
        bool BoundingBox::intersects (BoundingEllipsoid &be) {
            return be.intersects(*this);
        }
        //Test if the box intersects another.
        bool BoundingBox::intersects (BoundingBox &other) {

            int hx1 = width * 0.5;
            int hy1 = height * 0.5;
            int hz1 = depth * 0.5;
            int hx2 = other.width * 0.5;
            int hy2 = other.height * 0.5;
            int hz2 = other.depth * 0.5;
            //Check the mins and max medians positions.
            float minX1 = center.x - hx1, minX2 = other.center.x - hx2;
            float minY1 = center.y - hy1, minY2 = other.center.y - hy2;
            float minZ1 = center.z - hz1, minZ2 = other.center.z - hz2;
            float maxX1 = center.x + hx1, maxX2 = other.center.x + hx2;
            float maxY1 = center.y + hy1, maxY2 = other.center.y + hy2;
            float maxZ1 = center.z + hz1, maxZ2 = other.center.z + hz2;
            //If the medians overlap, our two boxes intersects.
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
        //Test if an the box intersects with the specified oriented bounding box.
        bool BoundingBox::intersects(OrientedBoundingBox &obx) {
            return obx.intersects(*this);
        }
        //Test if the bounding box intersects with the specified bounding polyhedron.
        bool BoundingBox::intersects (BoundingPolyhedron &bp) {
             std::array<math::Vec3f, 6> hi;
             hi[0] = math::Vec3f(width * 0.5f, 0, 0);
             hi[1] = math::Vec3f(0, height * 0.5f, 0);
             hi[2] = math::Vec3f(0, 0, depth * 0.5f);
             hi[3] = math::Vec3f(-width * 0.5f, 0, 0);
             hi[4] = math::Vec3f(0, -height * 0.5f, 0);
             hi[5] = math::Vec3f(0, 0, -depth * 0.5f);
             std::vector<math::Vec3f> normals;
             std::vector<math::Vec3f> bissectors;
             bissectors = bp.get3DBissectors();
             normals = bp.get3DNormals();
             float distMin1 = 0, distMin2 = 0;
             int faceIndex1 = 0, faceIndex2 = 0;
             int ptIndex1 = math::Computer::checkNearestVertexFromShape(points, bissectors, distMin1, faceIndex2);
             int ptIndex2 = math::Computer::checkNearestVertexFromShape(bp.getPoints(), hi, distMin2, faceIndex1);
             if (distMin1 < distMin2) {
                   math::Vec3f bpn = (bissectors[faceIndex2] - bp.getCenter()).projOnVector(normals[faceIndex2]);
                   math::Vec3f d = points[ptIndex1] - bp.getCenter();
                   float p = d.projOnAxis(bpn);
                   if (p * p > bpn.magnSquared()) {
                      return false;
                   }
                   return true;
             } else {
                  math::Vec3f bpn = hi[faceIndex1];
                  math::Vec3f d = bp.getPoints()[ptIndex2] - center;
                  float p = d.projOnAxis(bpn);
                  if (p * p > bpn.magnSquared()) {
                     return false;
                  }
                  return true;
             }
        }
        //Test if a point is inside our box.
        bool BoundingBox::isPointInside (math::Vec3f point) {
            //Check the min and max values of the medians of our box.
            float minX = center.x - width * 0.5f;
            float maxX = center.x + width * 0.5f;
            float minY = center.y - height * 0.5f;
            float maxY = center.y + height * 0.5f;
            float minZ = center.z - depth * 0.5f;
            float maxZ = center.z + depth * 0.5f;
            //If one of the point is on one of the x, y or z medians, the point is inside the box.
            return (point.x >= minX && point.x <= maxX && point.y >= minY && point.y <= maxY && point.z >= minZ && point.z <= maxZ);
        }
        bool BoundingBox::intersects (math::Ray &ray) {
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
                    if (math::Math::abs(orig) > hi)
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
        bool BoundingBox::intersectsWhere (math::Ray &ray, math::Vec3f& i1, math::Vec3f& i2) {
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
                    if (math::Math::abs(orig) > hi)
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
            if (tNear > 0)
                i1 = ray.getOrig() + ray.getDir() * tNear;
            else
                i1 = ray.getOrig();
            i2 = ray.getOrig() + ray.getDir() * tFar;
            return true;

        }
        //Acceseurs.
        math::Vec3f BoundingBox::getCenter () {
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
        math::Vec3f BoundingBox::getSize() {
            return math::Vec3f(width, height, depth);
        }
        math::Vec3f BoundingBox::getPosition () {
            return math::Vec3f (x, y, z);
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
        BoundingBox BoundingBox::transform(graphic::TransformMatrix& tm) {
            std::array<math::Vec3f, 8> points;
            //Devant
            points[0] = getPosition();
            points[1] = math::Vec3f (getPosition().x + getWidth(), getPosition().y, getPosition().z);
            points[2] = math::Vec3f (getPosition().x + getWidth(), getPosition().y + getHeight(), getPosition().z);
            points[3] = math::Vec3f (getPosition().x, getPosition().y + getHeight(), getPosition().z);
            //Derrière
            points[4] = math::Vec3f (getPosition().x, getPosition().y, getPosition().z - getDepth());
            points[5] = math::Vec3f (getPosition().x, getPosition().y + getHeight(), getPosition().z - getDepth());
            points[6] = math::Vec3f (getPosition().x + getWidth(), getPosition().y + getHeight(), getPosition().z - getDepth());
            points[7] = math::Vec3f (getPosition().x + getWidth(), getPosition().y, getPosition().z - getDepth());
            //tm.setOrigin(center - getPosition());
            for (unsigned int i = 0; i < points.size(); i++) {
                points[i] = tm.transform(points[i]);

            }
            std::array<std::array<float, 2>, 3> store = math::Computer::getExtends(points);
            BoundingBox bx(store[0][0], store[1][0],store[2][0], store[0][1] - store[0][0],store[1][1] - store[1][0], store[2][1] - store[2][0]);
            /*std::cout<<bx.getPosition().x<<" "<<bx.getPosition().y<<" "<<bx.getPosition().z<<" "<<bx.getWidth()<<" "<<bx.getHeight()<<" "<<bx.getDepth()<<std::endl;
            std::string s;
            std::cin>>s;*/
            return bx;
        }
        void BoundingBox::move(math::Vec3f t) {
            graphic::TransformMatrix tm;
            tm.setOrigin(center);
            center = center + t;
            tm.setTranslation(center);
            for (unsigned int i = 0; i < points.size(); i++)
                points[i] = tm.transform(points[i]);
            std::array<std::array<float, 2>, 3> extends = math::Computer::getExtends(points);
            x = extends[0][0];
            y = extends[1][0];
            z = extends[2][0];
            width = extends[0][1] - extends[0][0];
            height = extends[1][1] - extends[1][0];
            depth = extends[2][1] - extends[2][0];
        }
        void BoundingBox::scale(math::Vec3f s) {
            graphic::TransformMatrix tm;
            tm.setScale(s);
            tm.setOrigin(center-getPosition());
            tm.setTranslation(center);
            for (unsigned int i = 0; i < points.size(); i++)
                points[i] = tm.transform(points[i]);
            std::array<std::array<float, 2>, 3> extends = math::Computer::getExtends(points);
            x = extends[0][0];
            y = extends[1][0];
            z = extends[2][0];
            width = extends[0][1] - extends[0][0];
            height = extends[1][1] - extends[1][0];
            depth = extends[2][1] - extends[2][0];
        }
        std::array<math::Vec3f, 8> BoundingBox::getVertices() {
            return points;
        }
    }
}
