#include "../../../include/odfaeg/Physics/boundingPolyhedron.h"
#include "../../../include/odfaeg/Physics/boundingSphere.h"
#include "../../../include/odfaeg/Physics/boundingEllipsoid.h"
#include "../../../include/odfaeg/Physics/boundingBox.h"
#include "../../../include/odfaeg/Physics/orientedBoundingBox.h"
#include "../../../include/odfaeg/Graphics/transformMatrix.h"
namespace odfaeg {
    namespace physic {
        BoundingPolyhedron::BoundingPolyhedron() {
        }
        BoundingPolyhedron::BoundingPolyhedron (math::Vec3f p1, math::Vec3f p2, math::Vec3f p3) {
            addPoint(p1);
            addPoint(p2);
            addPoint(p3);
        }
        bool BoundingPolyhedron::intersects(math::Ray& ray, bool segment) {
            for (unsigned int i = 0; i < points.size(); i++) {
                   math::Plane p(normals3D[i], bissectors3D[i]);
                   //The ray is parallal to the triangle, but not on the triangle => no intersections.
                   if (ray.getDir().dot2(normals3D[i]) == 0
                       && p.whichSide(ray.getOrig()) != 0) {
                       return false;
                   //The ray is parallal to the triangle, and on the triangle. => we check a 2D intersection.
                   } else if (ray.getDir().dot2(normals3D[i]) == 0
                              && p.whichSide(ray.getOrig()) == 0) {
                       math::Vec3f v1 = points[i];
                       math::Vec3f v2 = points[(i + 1 == points.size()) ? 0 : i + 1];
                       math::Ray r (v1, v2);
                       float intrs = r.intersectsWhere(ray);
                       if (intrs != -1) {
                           return true;
                       }
                  } else {
                       math::Vec3f pointA = points[i];
                       math::Vec3f pointB = points[(i+1 >= points.size()) ? 0 : i + 1];
                       math::Vec3f pointC;
                       math::Vec3f o, point;
                       if (i + 2 == points.size()) {
                           pointC = points[0];
                       } else if (i + 1 == points.size()) {
                           pointC = points[1];
                       } else {
                           pointC = points[i+2];
                       }
                       math::Vec3f r = ray.getOrig() - ray.getExt();
                       math::Vec3f u = pointB - pointA;
                       math::Vec3f v = pointC - pointA;
                       math::Vec3f n = u.cross(v);
                       math::Vec3f otr = ray.getOrig() - pointA;
                       point.x = n.dot2(otr) / n.dot2(r);
                       point.y = -otr.cross(u).dot2(r) / n.dot2(r);
                       point.z = -v.cross(otr).dot2(r) / n.dot2(r);
                       if (0 <= point.x
                       && 0 <= point.y && point.y <= 1
                       &&  0 <= point.z && point.z <= 1
                       && point.y + point.z <= 1) {
                           return true;
                       }
                  }
            }
            return false;
        }
        bool BoundingPolyhedron::intersectsWhere(math::Ray& ray, math::Vec3f& i1, math::Vec3f& i2) {
            int nbInt = 0;
            for (unsigned int i = 0; i < points.size(); i++) {
                   math::Plane p(normals3D[i], bissectors3D[i]);
                   //The ray is parallal to the triangle, but not on the triangle => no intersections.
                   if (ray.getDir().dot2(normals3D[i]) == 0
                       && p.whichSide(ray.getOrig()) != 0) {
                       return false;
                   //The ray is parallal to the triangle, and on the triangle. => we check a 2D intersection.
                   } else if (ray.getDir().dot2(normals3D[i]) == 0
                              && p.whichSide(ray.getOrig()) == 0) {
                        math::Vec3f v1 = points[i];
                        math::Vec3f v2 = points[(i + 1 == points.size()) ? 0 : i + 1];
                        math::Ray r (v1, v2);
                        float intrs = ray.intersectsWhere(r);
                        if (intrs != -1) {
                            if (nbInt == 0) {
                                i1 = ray.getOrig() + ray.getDir() * intrs;
                                nbInt++;
                            } else {
                                i2 = ray.getOrig() + ray.getDir() * intrs;;
                                nbInt++;
                            }
                        }
                   //The ray is not parallal to the trriangle, I use raytracing algorithm to find the intersection.
                   } else {
                       math::Vec3f pointA = points[i];
                       math::Vec3f pointB = points[(i+1 >= points.size()) ? 0 : i + 1];
                       math::Vec3f pointC;
                       if (i + 2 == points.size()) {
                           pointC = points[0];
                       } else if (i + 1 == points.size()) {
                           pointC = points[1];
                       } else {
                           pointC = points[i+2];
                       }
                       math::Vec3f o, point;
                       math::Vec3f r = ray.getOrig() - ray.getExt();
                       math::Vec3f u = pointB - pointA;
                       math::Vec3f v = pointC - pointA;
                       math::Vec3f n = u.cross(v);
                       math::Vec3f otr = ray.getOrig() - pointA;
                       point.x = n.dot2(otr) / n.dot2(r);
                       point.y = -otr.cross(u).dot2(r) / n.dot2(r);
                       point.z = -v.cross(otr).dot2(r) / n.dot2(r);
                       if (0 <= point.x
                       && 0 <= point.y && point.y <= 1
                       &&  0 <= point.z && point.z <= 1
                       && point.y + point.z <= 1) {
                           if (nbInt == 0) {
                               i1.x = pointA.x + u.x * point.z + v.x * point.y;
                               i1.y = pointA.y + u.y * point.z + v.y * point.y;
                               i1.z = pointA.z + u.z * point.z + v.z * point.y;
                               nbInt++;
                           } else {
                               i2.x = pointA.x + u.x * point.z + v.x * point.y;
                               i2.y = pointA.y + u.y * point.z + v.y * point.y;
                               i2.z = pointA.z + u.z * point.z + v.z * point.y;
                               nbInt++;
                           }
                      }
                  }
            }
            if (nbInt == 1) {
                i2 = i1;
                i1 = ray.getOrig();
            } else {
                if (i1.computeDistSquared(ray.getOrig()) > i2.computeDistSquared(ray.getOrig())) {
                    math::Vec3f tmp;
                    tmp = i1;
                    i1 = i2;
                    i2 = tmp;

                }
            }
            return (nbInt > 0) ? true : false;
        }
        void BoundingPolyhedron::addPoint(math::Vec3f point) {
            bool contains = false;
            for (unsigned int i = 0; i < points.size() && !contains; i++) {
                if (points[i] == point)
                    contains = true;
            }
            if (!contains) {
                points.push_back(point);
                computeVectors();
            }
            std::array<std::array<float, 2>, 3> store = math::Computer::getExtends(points);
            size = math::Vec3f(store[0][1] - store[0][0], store[1][1] - store[1][0], store[2][1] - store[2][0]);
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
            float distMin1 = 0, distMin2 = 0;
            int faceIndex1 = 0, faceIndex2 = 0;
            //Check the nearest vertex of the polygon 1 from the triangles of polygon 2 and gives the distance.
            int ptIndex1 = math::Computer::checkNearestVertexFromShape(points, bp.bissectors3D, distMin1, faceIndex2);
            //Check the nearest vertex of the polygon 2 from the triangles of polygon 1 and gives the distance.
            int ptIndex2 = math::Computer::checkNearestVertexFromShape(bp.points, bissectors3D, distMin2, faceIndex1);
            //Check the shorted distance to check the collision.
            if (distMin1 < distMin2) {
                //The triangle's plane.
                math::Plane plane(bp.normals3D[faceIndex2],bp.bissectors3D[faceIndex2]);
                //If the faces are parallal but not on the same plane => no intersections.
                if (normals3D[ptIndex1].dot(bp.normals3D[ptIndex2]) == 1
                    && plane.whichSide(points[ptIndex1]) != 0) {
                    return false;
                    //If the face are parallal, we need to check for a 2D intersection on the 2D plane.
                } else if (normals3D[ptIndex1].dot(bp.normals3D[ptIndex2]) == 1
                           && plane.whichSide(points[ptIndex1]) == 0) {
                    unsigned int nextPtsIndex = (ptIndex1 == points.size() - 1) ? 0 : ptIndex1 + 1;
                    unsigned int nextFaceIndex = (faceIndex2 == bp.bissectors2D.size() - 1) ? 0 : faceIndex2 + 1;
                    float dist1 = bp.bissectors2D[faceIndex2].computeDistSquared(points[ptIndex1]);
                    float dist2 = bp.bissectors2D[nextFaceIndex].computeDistSquared(points[nextPtsIndex]);
                    if (dist1 < dist2) {
                        math::Vec3f bpn = (bp.bissectors2D[faceIndex2] - bp.center).projOnVector(bp.normals2D[faceIndex2]);
                        math::Vec3f d = points[ptIndex1] - bp.center;
                        float p = d.projOnAxis(bpn);
                        if (p * p > bpn.magnSquared()) {
                            return false;
                        }
                        return true;
                    } else {
                        math::Vec3f bpn = (bp.bissectors2D[nextFaceIndex] - bp.center).projOnVector(bp.normals2D[nextFaceIndex]);
                        math::Vec3f d = points[nextPtsIndex] - bp.center;
                        float p = d.projOnAxis(bpn);
                        if (p * p > bpn.magnSquared()) {
                            return false;
                        }
                        return true;
                    }
                    //If the faces are not parallal, we need to check for a 3D intersection.
                } else {
                    math::Vec3f bpn = (bp.bissectors3D[faceIndex2] - bp.center).projOnVector(bp.normals3D[faceIndex2]);
                    math::Vec3f d = points[ptIndex1] - bp.center;
                    float p = d.projOnAxis(bpn);
                    if (p * p > bpn.magnSquared()) {
                        return false;
                    }
                    return true;
                }
            } else {
                //The triangle's plane.
                math::Plane plane(bp.normals3D[faceIndex1],bp.bissectors3D[faceIndex1]);
                //If the point is on the plane, we need to check for a 2D intersection on the 2D plane.
                if (plane.whichSide(points[ptIndex1]) == 0) {
                    unsigned int nextPtsIndex = (ptIndex2 == bp.points.size() - 1) ? 0 : ptIndex2 + 1;
                    unsigned int nextFaceIndex = (faceIndex1 == bissectors2D.size() - 1) ? 0 : faceIndex1 + 1;
                    float dist1 = bissectors2D[faceIndex1].computeDistSquared(bp.points[ptIndex2]);
                    float dist2 = bissectors2D[nextFaceIndex].computeDistSquared(points[nextPtsIndex]);
                    if (dist1 < dist2) {
                        math::Vec3f bpn = (bissectors2D[faceIndex1] - center).projOnVector(normals2D[faceIndex1]);
                        math::Vec3f d = bp.points[ptIndex2] - center;
                        float p = d.projOnAxis(bpn);
                        if (p * p > bpn.magnSquared()) {
                            return false;
                        }
                        return true;
                    } else {
                        math::Vec3f bpn = (bissectors2D[nextFaceIndex] - center).projOnVector(normals2D[nextFaceIndex]);
                        math::Vec3f d = bp.points[nextPtsIndex] - center;
                        float p = d.projOnAxis(bpn);
                        if (p * p > bpn.magnSquared()) {
                            return false;
                        }
                            return true;
                    }
                } else {
                    math::Vec3f bpn = (bissectors3D[faceIndex1] - center).projOnVector(normals3D[faceIndex1]);
                    math::Vec3f d = bp.points[ptIndex2] - center;
                    float p = d.projOnAxis(bpn);
                    if (p * p > bpn.magnSquared()) {
                        return false;
                    }
                    return true;
                }
            }
        }
        bool BoundingPolyhedron::isPointInside(math::Vec3f point) {
            float distMin = 0;
            unsigned int faceIndex = 0;
            std::vector<math::Vec3f> pts = {point};
            //Check the nearest face from the point.
            math::Computer::checkNearestVertexFromShape(pts, bissectors3D, distMin, faceIndex);
            math::Vec3f d = point - center;
             //The triangle's plane.
            math::Plane plane(normals3D[faceIndex],bissectors3D[faceIndex]);
            //If the point is on the face, we need to check for a 2D intersection on the 2D plane.
            if (plane.whichSide(point) == 0) {
                unsigned int nextFaceIndex = (faceIndex == bissectors2D.size() - 1) ? 0 : faceIndex + 1;
                float dist1 = bissectors2D[faceIndex].computeDistSquared(point);
                float dist2 = bissectors2D[nextFaceIndex].computeDistSquared(point);
                if (dist1 < dist2) {
                    math::Vec3f bpn = (bissectors2D[faceIndex] - center).projOnVector(normals2D[faceIndex]);
                    math::Vec3f d = point - center;
                    float p = d.projOnAxis(bpn);
                    if (p * p > bpn.magnSquared()) {
                       return false;
                    }
                    return true;
                } else {
                    math::Vec3f bpn = (bissectors2D[nextFaceIndex] - center).projOnVector(normals2D[nextFaceIndex]);
                    math::Vec3f d = point - center;
                    float p = d.projOnAxis(bpn);
                    if (p * p > bpn.magnSquared()) {
                       return false;
                    }
                    return true;
                }
            } else {
                math::Vec3f bpn = (bissectors3D[faceIndex] - center).projOnVector(normals3D[faceIndex]);
                float p = d.projOnAxis(bpn);
                if (p * p > bpn.magnSquared()) {
                    return false;
                }
                return true;
            }
        }
        int BoundingPolyhedron::nbPoints () {
            return points.size();
        }
        void BoundingPolyhedron::computeVectors () {
            math::Vec3f sum(0, 0, 0);
            for (unsigned int i = 0; i < points.size(); i++)
                sum += points[i];
            center = sum / points.size();
            normals3D.clear();
            normals2D.clear();
            bissectors3D.clear();
            bissectors2D.clear();
            for (unsigned int i = 0; i < points.size(); i++) {
                math::Vec3f a = points[i];
                math::Vec3f b = (i + 1 >= points.size()) ? points[0] : points[i+1];
                math::Vec3f c;
                if (i + 2 == points.size()) {
                    c = points[0];
                } else if (i + 1 == points.size()) {
                    c = points[1];
                } else {
                    c = points[i+2];
                }
                math::Vec3f bissector = (a + b + c) / 3;
                math::Vec3f v1 = b - a;
                math::Vec3f v2 = c - a;
                math::Vec3f n = v1.cross(v2);
                if (bissector.dot2(n) < 0)
                    n = -n;
                bissectors3D.push_back(bissector);
                normals3D.push_back(n);
                bissector = (v1 + v2) / 2;
                n = n.cross(v1);
                if (bissector.dot(n) < 0)
                    n = -n;
                bissectors2D.push_back(bissector);
                normals2D.push_back(n);
            }
        }
        std::vector<math::Vec3f> BoundingPolyhedron::get3DNormals() {
            return normals3D;
        }
        std::vector<math::Vec3f> BoundingPolyhedron::get2DNormals() {
            return normals2D;
        }
        std::vector<math::Vec3f> BoundingPolyhedron::get3DBissectors() {
            return bissectors3D;
        }
        std::vector<math::Vec3f> BoundingPolyhedron::get2DBissectors() {
            return bissectors2D;
        }
        std::vector<math::Vec3f> BoundingPolyhedron::getPoints() {
            return points;
        }
        BoundingPolyhedron::~BoundingPolyhedron () {
            normals3D.clear();
            normals2D.clear();
            bissectors3D.clear();
            bissectors2D.clear();
            points.clear();
        }
        bool BoundingPolyhedron::operator== (const BoundingPolyhedron &bp) {
            if(points.size() != bp.points.size())
                return false;
            for (unsigned int i = 0; i < points.size(); i++) {
                if (!(points[i] == bp.points[i])) {
                    return false;
                }
            }
            return true;
        }
        math::Vec3f BoundingPolyhedron::getPoint(unsigned  int index) {
            if (index >= 0 && index < points.size())
                return points[index];
            else
                return math::Vec3f (0, 0, 0);
        }

        math::Vec3f BoundingPolyhedron::getCenter () {
            return center;
        }
        math::Vec3f BoundingPolyhedron::getSize() {
            return size;
        }
        math::Vec3f BoundingPolyhedron::getPosition() {
            return math::Vec3f (center.x - size.x * 0.5f, center.y - size.y * 0.5f, center.z - size.z * 0.5f);
        }
        void BoundingPolyhedron::move(math::Vec3f v) {
            graphic::TransformMatrix tm;
            tm.setTranslation(v);
            tm.setOrigin(center);
            for (unsigned int i = 0; i < bissectors3D.size(); i++)
                bissectors3D[i] = tm.transform(bissectors3D[i]);
            for (unsigned int i = 0; i < bissectors2D.size(); i++)
                bissectors2D[i] = tm.transform(bissectors2D[i]);
            for (unsigned int i = 0; i < points.size(); i++)
                points[i] = tm.transform(points[i]);
            std::array<std::array<float, 2>, 3> store = math::Computer::getExtends(points);
            size = math::Vec3f(store[0][1] - store[0][0], store[1][1] - store[1][0], store[2][1] - store[2][0]);
        }
        void BoundingPolyhedron::scale(math::Vec3f s) {
            graphic::TransformMatrix tm;
            tm.setScale(s);
            tm.setOrigin(center);
            for (unsigned int i = 0; i < normals3D.size(); i++)
                normals3D[i] = tm.transform(normals3D[i]);
            for (unsigned int i = 0; i < normals2D.size(); i++)
                normals2D[i] = tm.transform(normals3D[i]);
            for (unsigned int i = 0; i < bissectors3D.size(); i++)
                bissectors3D[i] = tm.transform(bissectors3D[i]);
            for (unsigned int i = 0; i < bissectors2D.size(); i++)
                bissectors2D[i] = tm.transform(bissectors2D[i]);
            for (unsigned int i = 0; i < points.size(); i++)
                points[i] = tm.transform(points[i]);
            std::array<std::array<float, 2>, 3> store = math::Computer::getExtends(points);
            size = math::Vec3f(store[0][1] - store[0][0], store[1][1] - store[1][0], store[2][1] - store[2][0]);
        }
        void BoundingPolyhedron::rotate(float angle, math::Vec3f axis) {
            graphic::TransformMatrix tm;
            tm.setRotation(axis, angle);
            tm.setOrigin(center);
            for (unsigned int i = 0; i < normals3D.size(); i++)
                normals3D[i] = tm.transform(normals3D[i]);
            for (unsigned int i = 0; i < normals2D.size(); i++)
                normals2D[i] = tm.transform(normals3D[i]);
            for (unsigned int i = 0; i < bissectors3D.size(); i++)
                bissectors3D[i] = tm.transform(bissectors3D[i]);
            for (unsigned int i = 0; i < bissectors2D.size(); i++)
                bissectors2D[i] = tm.transform(bissectors2D[i]);
            for (unsigned int i = 0; i < points.size(); i++)
                points[i] = tm.transform(points[i]);
            std::array<std::array<float, 2>, 3> store = math::Computer::getExtends(points);
            size = math::Vec3f(store[0][1] - store[0][0], store[1][1] - store[1][0], store[2][1] - store[2][0]);
        }
    }
}


