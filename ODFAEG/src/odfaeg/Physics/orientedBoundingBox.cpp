#include "../../../include/odfaeg/Physics/boundingBox.h"
#include "../../../include/odfaeg/Physics/boundingSphere.h"
#include "../../../include/odfaeg/Physics/boundingEllipsoid.h"
#include "../../../include/odfaeg/Physics/orientedBoundingBox.h"
#include "../../../include/odfaeg/Physics/boundingPolyhedron.h"
#include "../../../include/odfaeg/Graphics/transformMatrix.h"
namespace odfaeg {
    namespace physic {
        OrientedBoundingBox::OrientedBoundingBox (math::Vec3f p1, math::Vec3f p2, math::Vec3f p3, math::Vec3f p4, math::Vec3f p5, math::Vec3f p6, math::Vec3f p7, math::Vec3f p8) {
            std::array<math::Vec3f, 8> corners;
            std::array<math::Vec3f, 4> frontCorners;
            std::array<math::Vec3f, 4> backCorners;
            corners[0] = p1;
            corners[1] = p2;
            corners[2] = p3;
            corners[3] = p4;
            corners[4] = p5;
            corners[5] = p6;
            corners[6] = p7;
            corners[7] = p8;
            for (unsigned int i = 0; i < corners.size() - 1; i++) {
                for (unsigned int j = 1; j < corners.size(); j++) {
                    if (corners[j].z < corners[i].z) {
                        float tmp = corners[i].z;
                        corners[i].z = corners[j].z;
                        corners[j].z = tmp;
                    }
                }
            }
            frontCorners[0] = corners[0];
            frontCorners[1] = corners[1];
            frontCorners[2] = corners[2];
            frontCorners[3] = corners[3];
            backCorners[0] = corners[4];
            backCorners[1] = corners[5];
            backCorners[2] = corners[6];
            backCorners[3] = corners[7];
            std::array<std::array<float, 2>, 3> store;
            store = math::Computer::getExtends(corners);
            x = store[0][0];
            y = store[1][0];
            z = store[2][0];
            store = math::Computer::getExtends(frontCorners);
            math::Vec3f position (store[0][0], store[1][0], store[2][0]);
            int p1Index=0, p2Index=0,  p3Index=0, p5Index=0;
            //Check the right order of the points to find the rays of the oriented bounding rectangle.
            for (unsigned int i = 0; i < 4; i++) {
                if (corners[i].x == position.x)
                    p1Index = i;
                else if (corners[i].y <= position.y && corners[i].x != store[0][1])
                    p2Index = i;
                else if (corners[i].y == store[1][1]) {

                } else
                    p3Index = i;
            }
            store = math::Computer::getExtends(backCorners);
            position = math::Vec3f (store[0][0], store[1][0], store[2][0]);
            //Check the right order of the points to find the rays of the oriented bounding rectangle.
            for (unsigned int i = 4; i < 8; i++) {
                if (corners[i].x == position.x)
                    p5Index = i;
            }
            bx = math::Vec3f (corners[p2Index] - corners[p1Index]);
            width = bx.magnitude();
            bx = bx.normalize();
            by = math::Vec3f (corners[p3Index] - corners[p2Index]);
            height = by.magnitude();
            by = by.normalize();
            bz = math::Vec3f (corners[p5Index] - corners[p1Index]);
            depth = bz.magnitude();
            bz = bz.normalize();
            center = math::Vec3f (x + width * 0.5f, y + height * 0.5f, z + depth * 0.5f);
        }
        OrientedBoundingBox::OrientedBoundingBox(math::Vec3f p1, math::Vec3f p2, math::Vec3f p3, math::Vec3f p4) {
            std::array<math::Vec3f, 4> corners;
            int p1Index=0, p2Index=0,  p3Index=0;
            std::array<std::array<float, 2>, 3> store;
            store = math::Computer::getExtends(corners);
            math::Vec3f position = math::Vec3f (store[0][0], store[1][0], store[2][0]);
            for (unsigned int i = 0; i < 4; i++) {
                if (corners[i].x == position.x)
                    p1Index = i;
                else if (corners[i].y <= position.y && corners[i].x != store[0][1])
                    p2Index = i;
                else if (corners[i].y == store[1][1]) {

                } else
                    p3Index = i;
            }
            bx = math::Vec3f (corners[p2Index] - corners[p1Index]);
            width = bx.magnitude();
            bx = bx.normalize();
            by = math::Vec3f (corners[p3Index] - corners[p2Index]);
            height = by.magnitude();
            by = by.normalize();
            bz = math::Vec3f (0, 0, 0);
            depth = bz.magnitude();
            bz = bz.normalize();
            center = math::Vec3f (x + width * 0.5f, y + height * 0.5f, z + depth * 0.5f);
        }
        bool OrientedBoundingBox::intersects(BoundingSphere &bs) {
            return bs.intersects(*this);
        }
        bool OrientedBoundingBox::intersects(BoundingEllipsoid &be) {
            return be.intersects(*this);
        }
        bool OrientedBoundingBox::intersects (math::Ray& ray, bool segment) {
            float tFar = INT_MAX;
            float tNear = -INT_MAX;
            int hi[3];
            hi[0] = getWidth() * 0.5f;
            hi[1] = getHeight() * 0.5f;
            hi[2] = getDepth() * 0.5f;
            math::Vec3f bi[3];
            bi[0] = this->bx * width * 0.5f;
            bi[1] = this->by * height * 0.5f;
            bi[2] = this->bz * depth * 0.5f;
            for (unsigned int i = 0; i < 3; i++) {
                math::Vec3f p1 = getCenter() + bi[i] * hi[i];
                math::Vec3f p2 = getCenter() - bi[i] * hi[i];
                float d1 = bi[i].dot2(ray.getDir());
                float d2 = -bi[i].dot2(ray.getDir());
                if (d1 != 0 && d2 !=0) {
                    float t1 = bi[i].dot2(p1 - ray.getOrig()) / d1;
                    float t2 = -bi[i].dot2(p2 - ray.getOrig()) / d2;
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
            }
            return true;
        }
        bool OrientedBoundingBox::intersectsWhere (math::Ray& ray, math::Vec3f& i1, math::Vec3f& i2) {
            float tFar = INT_MAX;
            float tNear = -INT_MAX;
            int hi[3];
            hi[0] = getWidth() * 0.5f;
            hi[1] = getHeight() * 0.5f;
            hi[2] = getDepth() * 0.5f;
            math::Vec3f bi[3];
            bi[0] = this->bx * width * 0.5f;
            bi[1] = this->by * height * 0.5f;
            bi[2] = this->bz * depth * 0.5f;
            for (unsigned int i = 0; i < 3; i++) {
                math::Vec3f p1 = getCenter() + bi[i] * hi[i];
                math::Vec3f p2 = getCenter() - bi[i] * hi[i];
                float d1 = bi[i].dot2(ray.getDir());
                float d2 = -bi[i].dot2(ray.getDir());
                if (d1 != 0 && d2 !=0) {
                    float t1 = bi[i].dot2(p1 - ray.getOrig()) / d1;
                    float t2 = -bi[i].dot2(p2 - ray.getOrig()) / d2;
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
            }
            if (tNear > 0)
                i1 = ray.getOrig() + ray.getDir() * tNear;
            else
                i1 = ray.getOrig();
            i2 = ray.getOrig() + ray.getDir() * tFar;
            return true;
        }
        bool OrientedBoundingBox::intersects (BoundingBox &bx) {
            std::array<math::Vec3f, 6> bissectors1 = {-this->bx * width * 0.5f,
            -this->by * height * 0.5f,
            -this->bz * depth * 0.5f,
            this->bx * width * 0.5f,
            this->by * height * 0.5f,
            this->bz * depth * 0.5f};
            std::array<math::Vec3f, 6> bissectors2 = {math::Vec3f(-bx.getWidth() * 0.5f, 0, 0),
            math::Vec3f(0, -bx.getHeight() * 0.5f,0),
            math::Vec3f(0, 0, -bx.getDepth() * 0.5f),
            math::Vec3f(bx.getCenter().x + bx.getWidth() * 0.5f, 0, 0),
            math::Vec3f(0, bx.getHeight() * 0.5f, 0),
            math::Vec3f(0, 0, bx.getDepth() * 0.5f)};
            unsigned int ptIndex1 = 0, ptIndex2 = 0, faceIndex1 = 0, faceIndex2 = 0;
            float distMin1, distMin2;
            ptIndex1 = math::Computer::checkNearestVertexFromShape(points, bissectors2,distMin1,faceIndex2);
            ptIndex2 = math::Computer::checkNearestVertexFromShape(bx.getVertices(), bissectors1,distMin2,faceIndex1);
            if (distMin1 < distMin2) {
                math::Vec3f d = points[ptIndex1] - bx.getCenter();
                float p = d.projOnAxis(bissectors2[faceIndex2]);
                if (p * p > bissectors2[faceIndex2].magnSquared())
                    return false;
                return true;
            } else {
                math::Vec3f d = bx.getVertices()[ptIndex2] - center;
                float p = d.projOnAxis(bissectors1[faceIndex1]);
                if (p * p > bissectors1[faceIndex1].magnSquared())
                    return false;
                return true;
            }
        }
        bool OrientedBoundingBox::intersects (OrientedBoundingBox &obx) {
            std::array<math::Vec3f, 6> bi1;
            bi1[0] = bx * width * 0.5f;
            bi1[1] = by * height * 0.5f;
            bi1[2] = bz * depth * 0.5f;
            bi1[3] = -bx * width * 0.5f;
            bi1[4] = -by * height * 0.5f;
            bi1[5] = -bz * depth * 0.5f;
            std::array<math::Vec3f, 6> bi2;
            bi2[0] = obx.getBX() * obx.getWidth() * 0.5f;
            bi2[1] = obx.getBY() * obx.getHeight() * 0.5f;
            bi2[2] = obx.getBZ() * obx.getDepth() * 0.5f;
            bi2[3] = -obx.getBX() * obx.getWidth() * 0.5f;
            bi2[4] = -obx.getBY() * obx.getHeight() * 0.5f;
            bi2[5] = -obx.getBZ() * obx.getDepth() * 0.5f;
            unsigned int ptIndex1 = 0, ptIndex2 = 0, faceIndex1 = 0, faceIndex2 = 0;
            float distMin1, distMin2;
            ptIndex1 = math::Computer::checkNearestVertexFromShape(points, bi2, distMin1, faceIndex2);
            ptIndex2 = math::Computer::checkNearestVertexFromShape(obx.getVertices(), bi1, distMin2, faceIndex1);
            if (distMin1 < distMin2) {
                math::Vec3f bpn = bi2[faceIndex2];
                math::Vec3f d = points[ptIndex1] - obx.getCenter();
                float p = d.projOnAxis(bpn);
                if (p * p > bpn.magnSquared()) {
                    return false;
                }
                return true;
            } else {
                math::Vec3f bpn = bi1[faceIndex1];
                math::Vec3f d = obx.getVertices()[ptIndex2] - center;
                float p = d.projOnAxis(bpn);
                if (p * p > bpn.magnSquared()) {
                    return false;
                }
                return true;
            }
        }
        bool OrientedBoundingBox::intersects (BoundingPolyhedron &bp) {
            std::array<math::Vec3f, 6> bi;
            bi[0] = bx * width * 0.5f;
            bi[1] = by * height * 0.5f;
            bi[2] = bz * depth * 0.5f;
            bi[3] = -bx * width * 0.5f;
            bi[4] = -by * height * 0.5f;
            bi[5] = -bz * depth * 0.5f;
            float distMin1 = 0, distMin2 = 0;
            int faceIndex1 = 0, faceIndex2 = 0;
            std::vector<math::Vec3f> normals;
            std::vector<math::Vec3f> bissectors;
            bissectors = bp.get3DBissectors();
            normals = bp.get3DNormals();
            int ptIndex1 = math::Computer::checkNearestVertexFromShape(points, bissectors, distMin1, faceIndex2);
            int ptIndex2 = math::Computer::checkNearestVertexFromShape(bp.getPoints(), bi, distMin2, faceIndex1);
            if (distMin1 < distMin2) {
                math::Vec3f bpn = (bissectors[faceIndex2] - bp.getCenter()).projOnVector(normals[faceIndex2]);
                math::Vec3f d = points[ptIndex1] - bp.getCenter();
                float p = d.projOnAxis(bpn);
                if (p * p > bpn.magnSquared()) {
                    return false;
                }
                return true;
            } else {
                math::Vec3f bpn = bi[faceIndex1];
                math::Vec3f d = bp.getPoints()[ptIndex2] - center;
                float p = d.projOnAxis(bpn);
                if (p * p > bpn.magnSquared()) {
                    return false;
                }
                return true;
            }
        }

        math::Vec3f OrientedBoundingBox::getCenter() {
            return center;
        }
        int OrientedBoundingBox::getWidth() {
            return width;
        }
        int OrientedBoundingBox::getHeight() {
            return height;
        }
        int OrientedBoundingBox::getDepth() {
            return depth;
        }
        math::Vec3f OrientedBoundingBox::getSize() {
            return math::Vec3f(width, height, depth);
        }
        math::Vec3f OrientedBoundingBox::getPosition() {
            return math::Vec3f(x, y, z);
        }
        void OrientedBoundingBox::setCorners (math::Vec3f p1, math::Vec3f p2, math::Vec3f p3, math::Vec3f p4, math::Vec3f p5, math::Vec3f p6, math::Vec3f p7, math::Vec3f p8) {
            std::array<math::Vec3f, 8> corners;
            std::array<math::Vec3f, 4> frontCorners;
            std::array<math::Vec3f, 4> backCorners;
            corners[0] = p1;
            corners[1] = p2;
            corners[2] = p3;
            corners[3] = p4;
            corners[4] = p5;
            corners[5] = p6;
            corners[6] = p7;
            corners[7] = p8;
            for (unsigned int i = 0; i < corners.size() - 1; i++) {
                for (unsigned int j = 1; j < corners.size(); j++) {
                    if (corners[j].z < corners[i].z) {
                        float tmp = corners[i].z;
                        corners[i].z = corners[j].z;
                        corners[j].z = tmp;
                    }
                }
            }
            frontCorners[0] = corners[0];
            frontCorners[1] = corners[1];
            frontCorners[2] = corners[2];
            frontCorners[3] = corners[3];
            backCorners[0] = corners[4];
            backCorners[1] = corners[5];
            backCorners[2] = corners[6];
            backCorners[3] = corners[7];
            std::array<std::array<float, 2>, 3> store;
            store = math::Computer::getExtends(corners);
            x = store[0][0];
            y = store[1][0];
            z = store[2][0];
            store = math::Computer::getExtends(frontCorners);
            math::Vec3f position (store[0][0], store[1][0], store[2][0]);
            int p1Index=0, p2Index=0,  p3Index=0, p5Index=0;
            //Check the right order of the points to find the rays of the oriented bounding rectangle.
            for (unsigned int i = 0; i < 4; i++) {
                if (corners[i].x == position.x)
                    p1Index = i;
                else if (corners[i].y <= position.y && corners[i].x != store[0][1])
                    p2Index = i;
                else if (corners[i].y == store[1][1]) {

                } else
                    p3Index = i;
            }
            store = math::Computer::getExtends(backCorners);
            position = math::Vec3f (store[0][0], store[1][0], store[2][0]);
            //Check the right order of the points to find the rays of the oriented bounding rectangle.
            for (unsigned int i = 4; i < 8; i++) {
                if (corners[i].x == position.x)
                    p5Index = i;
            }
            bx = math::Vec3f (corners[p2Index] - corners[p1Index]);
            width = bx.magnitude();
            bx = bx.normalize();
            by = math::Vec3f (corners[p3Index] - corners[p2Index]);
            height = by.magnitude();
            by = by.normalize();
            bz = math::Vec3f (corners[p5Index] - corners[p1Index]);
            depth = bz.magnitude();
            bz = bz.normalize();
            center = math::Vec3f (x + width * 0.5f, y + height * 0.5f, z + depth * 0.5f);
        }
        math::Vec3f OrientedBoundingBox::getBX() {
            return bx;
        }
        math::Vec3f OrientedBoundingBox::getBY() {
            return by;
        }
        math::Vec3f OrientedBoundingBox::getBZ() {
            return bz;
        }
        void OrientedBoundingBox::move(math::Vec3f v) {
            graphic::TransformMatrix tm;
            tm.setTranslation(v);
            tm.setOrigin(center);
            center = tm.transform(center);
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
        void OrientedBoundingBox::scale(math::Vec3f v) {
            graphic::TransformMatrix tm;
            tm.setScale(v);
            tm.setOrigin(center);
            for (unsigned int i = 0; i < points.size(); i++)
                points[i] = tm.transform(points[i]);
            bx = tm.transform(bx);
            by = tm.transform(by);
            bz = tm.transform(bz);
            std::array<std::array<float, 2>, 3> extends = math::Computer::getExtends(points);
            x = extends[0][0];
            y = extends[1][0];
            z = extends[2][0];
            width = extends[0][1] - extends[0][0];
            height = extends[1][1] - extends[1][0];
            depth = extends[2][1] - extends[2][0];
        }
        void OrientedBoundingBox::rotate(float angle, math::Vec3f v) {
            graphic::TransformMatrix tm;
            tm.setRotation(v, angle);
            tm.setOrigin(center);
            for (unsigned int i = 0; i < points.size(); i++)
                points[i] = tm.transform(points[i]);
            bx = tm.transform(bx);
            by = tm.transform(by);
            bz = tm.transform(bz);
            std::array<std::array<float, 2>, 3> extends = math::Computer::getExtends(points);
            x = extends[0][0];
            y = extends[1][0];
            z = extends[2][0];
            width = extends[0][1] - extends[0][0];
            height = extends[1][1] - extends[1][0];
            depth = extends[2][1] - extends[2][0];
        }
    }
}
