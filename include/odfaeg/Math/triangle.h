#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP
#include "plane.h"
namespace odfaeg {
    namespace math {
        class Triangle {
        public :
            Triangle (Vec3f p1, Vec3f p2, Vec3f p3) :
            p1(p1), p2(p2), p3(p3) {

            }
            bool intersects(math::Ray& ray) {
               Vec3f n = p1.cross(p2);
               Plane p(n, p1);
               //The ray is parallal to the triangle, but isn't on the triangle => no intersections.
               if (ray.getDir().dot2(n) == 0
                   && p.whichSide(ray.getOrig()) != 0) {
                   return false;
               //The ray is parallal to the triangle, and is on the triangle. => we check a 2D intersection.
               } else if (ray.getDir().dot2(n) == 0
                          && p.whichSide(ray.getOrig()) == 0) {
                   Ray r (p1, p2);
                   float intrs = r.intersectsWhere(ray);
                   if (intrs != -1) {
                       return true;
                   }
                   r = Ray (p2, p3);
                   intrs = r.intersectsWhere(ray);
                   if (intrs != -1) {
                       return true;
                   }
                   r = Ray (p3, p1);
                   intrs = r.intersectsWhere(ray);
                   if (intrs != -1) {
                       return true;
                   }
              } else {
                   Vec3f o, point;
                   Vec3f r = ray.getOrig() - ray.getExt();
                   Vec3f u = p2 - p1;
                   Vec3f v = p3 - p1;
                   Vec3f n = u.cross(v);
                   Vec3f otr = ray.getOrig() - p1;
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
              return false;
        }
        bool intersectsWhere(math::Ray& ray, math::Vec3f& i1, math::Vec3f& i2) {
           int nbInt = 0;
           Vec3f n = p1.cross(p2);
           math::Plane p(n, p1);
           //The ray is parallal to the triangle, but isn't on the triangle => no intersections.
           if (ray.getDir().dot2(n) == 0
               && p.whichSide(ray.getOrig()) != 0) {
               return false;
           //The ray is parallal to the triangle, and is on the triangle. => we check a 2D intersection.
           } else if (ray.getDir().dot2(n) == 0
                      && p.whichSide(ray.getOrig()) == 0) {
                    Ray r (p1, p2);
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
                    r = Ray (p2, p3);
                    intrs = ray.intersectsWhere(r);
                    if (intrs != -1) {
                        if (nbInt == 0) {
                            i1 = ray.getOrig() + ray.getDir() * intrs;
                            nbInt++;
                        } else {
                            i2 = ray.getOrig() + ray.getDir() * intrs;;
                            nbInt++;
                        }
                    }
                    r = Ray (p3, p1);
                    intrs = ray.intersectsWhere(r);
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

                   Vec3f o, point;
                   Vec3f r = ray.getOrig() - ray.getExt();
                   Vec3f u = p2 - p1;
                   Vec3f v = p3 - p1;
                   Vec3f n = u.cross(v);
                   Vec3f otr = ray.getOrig() - p1;
                   point.x = n.dot2(otr) / n.dot2(r);
                   point.y = -otr.cross(u).dot2(r) / n.dot2(r);
                   point.z = -v.cross(otr).dot2(r) / n.dot2(r);
                   if (0 <= point.x
                   && 0 <= point.y && point.y <= 1
                   &&  0 <= point.z && point.z <= 1
                   && point.y + point.z <= 1) {
                       if (nbInt == 0) {
                           i1.x = p1.x + u.x * point.z + v.x * point.y;
                           i1.y = p1.y + u.y * point.z + v.y * point.y;
                           i1.z = p1.z + u.z * point.z + v.z * point.y;
                           nbInt++;
                       } else {
                           i2.x = p1.x + u.x * point.z + v.x * point.y;
                           i2.y = p1.y + u.y * point.z + v.y * point.y;
                           i2.z = p1.z + u.z * point.z + v.z * point.y;
                           nbInt++;
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
        bool intersects (Triangle other) {
            Ray r1 (p1, p2);
            Ray r2 (p2, p1);
            Ray r3 (p3, p1);
            Ray r4 (other.p1, other.p2);
            Ray r5 (other.p2, other.p3);
            Ray r6 (other.p3, other.p1);
            float i;
            Plane pl1 = (!other.p1.isNulVector()) ? Plane(other.p1.cross(other.p2), other.p1) : Plane(other.p2.cross(other.p3), other.p2);
            Plane pl2 = (!p1.isNulVector()) ? Plane (p1.cross(p2), p1) : Plane (p2.cross(p3), p2);
            if (((pl1.intersects(r1, i) && i <= 1)
                || (pl1.intersects(r2, i) && i <= 1)
                || (pl1.intersects(r3, i) && i <= 1))
                && ((pl2.intersects(r4, i) && i <= 1)
                || (pl2.intersects(r5, i) && i <= 1)
                || (pl2.intersects(r6, i) && i <= 1)))
               return true;
            return false;
        }
        Vec3f getP1() {
            return p1;
        }
        Vec3f getP2() {
            return p2;
        }
        Vec3f getP3() {
            return p3;
        }
        private :
            Vec3f p1, p2, p3;
        };
    }
}
#endif // TRIANGLE_HPP
