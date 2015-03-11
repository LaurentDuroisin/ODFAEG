#include "../../../include/odfaeg/Physics/boundingPolyhedron.h"
#include "../../../include/odfaeg/Physics/boundingSphere.h"
#include "../../../include/odfaeg/Physics/boundingEllipsoid.h"
#include "../../../include/odfaeg/Physics/boundingBox.h"
#include "../../../include/odfaeg/Physics/orientedBoundingBox.h"
#include "../../../include/odfaeg/Graphics/transformMatrix.h"
namespace odfaeg {
    namespace physic {
        BoundingPolyhedron::BoundingPolyhedron() {
            flat = true;
        }
        BoundingPolyhedron::BoundingPolyhedron (math::Vec3f p1, math::Vec3f p2, math::Vec3f p3, bool flat) {
            this->flat = flat;
            points.push_back(p1);
            points.push_back(p2);
            points.push_back(p3);
        }
        bool BoundingPolyhedron::intersects(math::Ray& ray, bool segment) {
            for (unsigned int i = 0; i < points.size(); i+=3) {
                   math::Triangle t (points[i], points[i+1], points[i+2]);
                   if(t.intersects(ray))
                    return true;
            }
            return false;
        }
        bool BoundingPolyhedron::intersectsWhere(math::Ray& ray, math::Vec3f& i1, math::Vec3f& i2) {
            for (unsigned int i = 0; i < points.size(); i+=3) {
                   math::Triangle t (points[i], points[i+1], points[i+2]);
                   if(t.intersectsWhere(ray, i1, i2))
                      return true;
            }
            return false;
        }
        void BoundingPolyhedron::addTriangle(math::Vec3f p1, math::Vec3f p2, math::Vec3f p3) {
            points.push_back(p1);
            points.push_back(p2);
            points.push_back(p3);
            computeVectors();
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
            float distMin1, distMin2;
            int index1, index2;
            int edgeIndex1, edgeIndex2;
            int faceIndex1, faceIndex2;
            /* Check the nearest vertex of the polygon 2 from the regions of polygon 2 and give the distance and
            *  return the point or the face's bissector of the region.
            */
            int ptIndex1 = math::Computer::checkNearestVertexFromShape(center, points, edgeBissectors, edgeNormals, faceBissectors, faceNormals, bp.points, distMin1, index1, edgeIndex1, faceIndex1);
            /* Check the nearest vertex of the polygon 1 from the regions of polygon 2 and give the distance and
            *  return the point or face's bissector of the region.
            */
            int ptIndex2 = math::Computer::checkNearestVertexFromShape(bp.center, bp.points, bp.edgeBissectors, bp.edgeNormals, bp.faceBissectors, faceNormals, points, distMin2, index2, edgeIndex1, faceIndex1);
            if (index1 != -1)
                return (points[index1] - center).magnSquared() >= (bp.points[ptIndex1] - center).magnSquared();
            if (index2 != -1)
                return (points[index2] - center).magnSquared() >= (bp.points[ptIndex2] - center).magnSquared();
            if (flat && bp.flat) {
                if (faceIndex1 != -1 && faceIndex2 != -1)  {
                    math::Triangle t1(points[faceIndex1*3], points[faceIndex1*3+1], points[faceIndex1*3+2]);
                    math::Triangle t2(bp.points[faceIndex1*3], bp.points[faceIndex1*3+1], bp.points[faceIndex1*3+2]);
                    return t1.intersects(t2);
                }
                if (faceIndex1 != -1 && faceIndex2 == -1) {
                    math::Triangle t(points[faceIndex1*3], points[faceIndex1*3+1], points[faceIndex1*3+2]);
                    math::Ray r1 (points[edgeIndex2], points[(edgeIndex2 % 3 == 2) ? edgeIndex2 - 2 : edgeIndex2 + 1]);
                    math::Ray r2 (points[(edgeIndex2 % 3 == 2) ? edgeIndex2 - 2 : edgeIndex2 + 1], points[edgeIndex2]);
                    return (t.intersects(r1) && t.intersects(r2));
                }
                if (faceIndex1 == -1 && faceIndex2 != -1) {
                    math::Triangle t(points[faceIndex2*3], points[faceIndex2*3+1], points[faceIndex2*3+2]);
                    math::Ray r1 (points[edgeIndex1], points[(edgeIndex1 % 3 == 2) ? edgeIndex1 - 2 : edgeIndex1 + 1]);
                    math::Ray r2 (points[(edgeIndex1 % 3 == 2) ? edgeIndex1 - 2 : edgeIndex1 + 1], points[edgeIndex1]);
                    return (t.intersects(r1) && t.intersects(r2));
                }
                return false;
            }
            if (!flat && !bp.flat) {
                if (distMin1 < distMin2) {
                    if (faceIndex1 != -1) {
                        math::Vec3f bpn = (faceBissectors[faceIndex1] - center).projOnVector(faceNormals[faceIndex1]);
                        math::Vec3f d = points[ptIndex1] - center;
                        float p = d.projOnAxis(bpn);
                        if (p * p > bpn.magnSquared()) {
                            return false;
                        }
                        return true;
                    }
                    math::Vec3f bpn = (edgeBissectors[edgeIndex1] - center).projOnVector(edgeNormals[edgeIndex1]);
                    math::Vec3f d = points[ptIndex1] - center;
                    float p = d.projOnAxis(bpn);
                    return p * p > bpn.magnSquared();
                }
                if (faceIndex2 != -1) {
                    math::Vec3f bpn = (faceBissectors[faceIndex2] - center).projOnVector(faceNormals[faceIndex2]);
                    math::Vec3f d = points[ptIndex2] - center;
                    float p = d.projOnAxis(bpn);
                    return p * p > bpn.magnSquared();
                }
                math::Vec3f bpn = (edgeBissectors[edgeIndex2] - center).projOnVector(edgeNormals[edgeIndex2]);
                math::Vec3f d = points[ptIndex2] - center;
                float p = d.projOnAxis(bpn);
                return p * p > bpn.magnSquared();
            }
            if (flat && !bp.flat) {
                if (faceIndex1 != -1 && faceIndex2 != -1)  {
                    math::Triangle t1(points[faceIndex1*3], points[faceIndex1*3+1], points[faceIndex1*3+2]);
                    math::Triangle t2(bp.points[faceIndex2*3], bp.points[faceIndex2*3+1], bp.points[faceIndex2*3+2]);
                    return bp.isPointInside(t1.getP1())
                            || bp.isPointInside(t1.getP2())
                            || bp.isPointInside(t1.getP3())
                            || t1.intersects(t2);
                }
                if (faceIndex1 != -1 && faceIndex2 == -1) {
                    math::Triangle t(points[faceIndex1*3], points[faceIndex1*3+1], points[faceIndex1*3+2]);
                    math::Ray r1 (points[edgeIndex2], points[(edgeIndex2 % 3 == 2) ? edgeIndex2 - 2 : edgeIndex2 + 1]);
                    math::Ray r2 (points[(edgeIndex2 % 3 == 2) ? edgeIndex2 - 2 : edgeIndex2 + 1], points[edgeIndex2]);
                    return bp.isPointInside(t.getP1())
                           || bp.isPointInside(t.getP2())
                           || bp.isPointInside(t.getP3())
                           || (t.intersects(r1) && t.intersects(r2));
                }
                if (faceIndex1 == -1 && faceIndex2 != -1) {
                    math::Triangle t(points[faceIndex2*3], points[faceIndex2*3+1], points[faceIndex2*3+2]);
                    math::Ray r1 (points[edgeIndex1], points[(edgeIndex1 % 3 == 2) ? edgeIndex1 - 2 : edgeIndex1 + 1]);
                    math::Ray r2 (points[(edgeIndex1 % 3 == 2) ? edgeIndex1 - 2 : edgeIndex1 + 1], points[edgeIndex1]);
                    return bp.isPointInside(r1.getOrig())
                            || bp.isPointInside(r1.getExt())
                            || (t.intersects(r1) && t.intersects(r2));
                }
                return false;
            }
            if (!flat && bp.flat) {
                if (faceIndex1 != -1 && faceIndex2 != -1)  {
                    math::Triangle t1(points[faceIndex1*3], points[faceIndex1*3+1], points[faceIndex1*3+2]);
                    math::Triangle t2(bp.points[faceIndex2*3], bp.points[faceIndex2*3+1], bp.points[faceIndex2*3+2]);
                    return isPointInside(t2.getP1())
                            || bp.isPointInside(t2.getP2())
                            || bp.isPointInside(t2.getP3())
                            || t1.intersects(t2);
                }
                if (faceIndex1 != -1 && faceIndex2 == -1) {
                    math::Triangle t(points[faceIndex1*3], points[faceIndex1*3+1], points[faceIndex1*3+2]);
                    math::Ray r1 (points[edgeIndex2], points[(edgeIndex2 % 3 == 2) ? edgeIndex2 - 2 : edgeIndex2 + 1]);
                    math::Ray r2 (points[(edgeIndex2 % 3 == 2) ? edgeIndex2 - 2 : edgeIndex2 + 1], points[edgeIndex2]);
                    return bp.isPointInside(r1.getOrig())
                           || bp.isPointInside(r1.getExt())
                           || (t.intersects(r1) && t.intersects(r2));
                }
                if (faceIndex1 == -1 && faceIndex2 != -1) {
                    math::Triangle t(points[faceIndex2*3], points[faceIndex2*3+1], points[faceIndex2*3+2]);
                    math::Ray r1 (points[edgeIndex1], points[(edgeIndex1 % 3 == 2) ? edgeIndex1 - 2 : edgeIndex1 + 1]);
                    math::Ray r2 (points[(edgeIndex1 % 3 == 2) ? edgeIndex1 - 2 : edgeIndex1 + 1], points[edgeIndex1]);
                    return bp.isPointInside(t.getP1())
                            || bp.isPointInside(t.getP2())
                            || bp.isPointInside(t.getP3())
                            || (t.intersects(r1) && t.intersects(r2));
                }
                return false;
            }
        }
        bool BoundingPolyhedron::isPointInside(math::Vec3f point) {
            float distMin;
            int index;
            int edgeIndex, faceIndex;
            std::vector<math::Vec3f> vertices = {point};
            //Check the nearest face from the point.
            int ptIndex = math::Computer::checkNearestVertexFromShape(center, points, edgeBissectors, edgeNormals, faceBissectors, faceNormals, vertices, distMin, index,edgeIndex, faceIndex);
            if (index != -1) {
                return (points[index] - center).magnSquared() >= (vertices[ptIndex] - center).magnSquared();
            }
            if (flat) {
                if (faceIndex == -1)
                    return false;
                math::Plane p(faceNormals[faceIndex], faceBissectors[faceIndex]);
                if (p.whichSide(point) == 0)
                    return true;
                return false;
            }

            math::Vec3f bpn = (faceBissectors[faceIndex] - center).projOnVector(faceNormals[faceIndex]);
            math::Vec3f d = point - center;
            float p = d.projOnAxis(bpn);
            if (p * p > bpn.magnSquared()) {
                return false;
            }
            return true;
        }
        bool BoundingPolyhedron::isFlat() {
            return flat;
        }
        int BoundingPolyhedron::nbPoints () {
            return points.size();
        }
        void BoundingPolyhedron::computeVectors () {
            math::Vec3f sum(0, 0, 0);
            for (unsigned int i = 0; i < points.size(); i++)
                sum += points[i];
            center = sum / points.size();
            faceNormals.clear();
            edgeNormals.clear();
            faceBissectors.clear();
            edgeBissectors.clear();
            for (unsigned int i = 0; i < points.size(); i+=3) {
                math::Vec3f a = points[i];
                math::Vec3f b = points[i+1];
                math::Vec3f c = points[i+2];
                math::Vec3f bissector = (a + b + c) / 3;
                math::Vec3f v1 = b - a;
                math::Vec3f v2 = c - a;
                math::Vec3f n = v1.cross(v2);
                if (n.dot2(bissector - center) < 0)
                    n = -n;
                faceBissectors.push_back(bissector);
                faceNormals.push_back(n);
                bissector = (a + b) * 0.5f;
                n = n.cross(b - a);
                if (n.dot2(bissector - center) < 0)
                    n = -n;
                edgeBissectors.push_back(bissector);
                edgeNormals.push_back(n);
                bissector = (b + c) * 0.5f;
                n = n.cross(c - b);
                if (n.dot2(bissector - center) < 0)
                    n = -n;
                edgeBissectors.push_back(bissector);
                edgeNormals.push_back(n);
                bissector = (c + a) * 0.5f;
                n = n.cross(a - c);
                if (n.dot2(bissector - center) < 0)
                    n = -n;
                edgeBissectors.push_back(bissector);
                edgeNormals.push_back(n);
            }
        }
        std::vector<math::Vec3f> BoundingPolyhedron::get3DNormals() {
            return faceNormals;
        }
        std::vector<math::Vec3f> BoundingPolyhedron::get2DNormals() {
            return edgeNormals;
        }
        std::vector<math::Vec3f> BoundingPolyhedron::get3DBissectors() {
            return faceBissectors;
        }
        std::vector<math::Vec3f> BoundingPolyhedron::get2DBissectors() {
            return edgeBissectors;
        }
        std::vector<math::Vec3f> BoundingPolyhedron::getPoints() {
            return points;
        }
        BoundingPolyhedron::~BoundingPolyhedron () {
            faceNormals.clear();
            edgeNormals.clear();
            faceBissectors.clear();
            edgeBissectors.clear();
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
            for (unsigned int i = 0; i < faceBissectors.size(); i++)
                faceBissectors[i] = tm.transform(faceBissectors[i]);
            for (unsigned int i = 0; i < edgeBissectors.size(); i++)
                edgeBissectors[i] = tm.transform(edgeBissectors[i]);
            for (unsigned int i = 0; i < points.size(); i++)
                points[i] = tm.transform(points[i]);
            std::array<std::array<float, 2>, 3> store = math::Computer::getExtends(points);
            size = math::Vec3f(store[0][1] - store[0][0], store[1][1] - store[1][0], store[2][1] - store[2][0]);
        }
        void BoundingPolyhedron::scale(math::Vec3f s) {
            graphic::TransformMatrix tm;
            tm.setScale(s);
            tm.setOrigin(center);
            for (unsigned int i = 0; i < faceNormals.size(); i++)
                faceNormals[i] = tm.transform(faceNormals[i]);
            for (unsigned int i = 0; i < edgeNormals.size(); i++)
                edgeNormals[i] = tm.transform(faceNormals[i]);
            for (unsigned int i = 0; i < faceBissectors.size(); i++)
                faceBissectors[i] = tm.transform(faceBissectors[i]);
            for (unsigned int i = 0; i < edgeBissectors.size(); i++)
                edgeBissectors[i] = tm.transform(edgeBissectors[i]);
            for (unsigned int i = 0; i < points.size(); i++)
                points[i] = tm.transform(points[i]);
            std::array<std::array<float, 2>, 3> store = math::Computer::getExtends(points);
            size = math::Vec3f(store[0][1] - store[0][0], store[1][1] - store[1][0], store[2][1] - store[2][0]);
        }
        void BoundingPolyhedron::rotate(float angle, math::Vec3f axis) {
            graphic::TransformMatrix tm;
            tm.setRotation(axis, angle);
            tm.setOrigin(center);
            for (unsigned int i = 0; i < faceNormals.size(); i++)
                faceNormals[i] = tm.transform(faceNormals[i]);
            for (unsigned int i = 0; i < edgeNormals.size(); i++)
                edgeNormals[i] = tm.transform(faceNormals[i]);
            for (unsigned int i = 0; i < faceBissectors.size(); i++)
                faceBissectors[i] = tm.transform(faceBissectors[i]);
            for (unsigned int i = 0; i < edgeBissectors.size(); i++)
                edgeBissectors[i] = tm.transform(edgeBissectors[i]);
            for (unsigned int i = 0; i < points.size(); i++)
                points[i] = tm.transform(points[i]);
            std::array<std::array<float, 2>, 3> store = math::Computer::getExtends(points);
            size = math::Vec3f(store[0][1] - store[0][0], store[1][1] - store[1][0], store[2][1] - store[2][0]);
        }
    }
}


