#include "../../../include/odfaeg/Physics/boundingSphere.h"
#include "../../../include/odfaeg/Physics/boundingEllipsoid.h"
#include "../../../include/odfaeg/Physics/boundingBox.h"
#include "../../../include/odfaeg/Physics/orientedBoundingBox.h"
#include "../../../include/odfaeg/Physics/boundingPolyhedron.h"
#include "../../../include/odfaeg/Graphics/transformMatrix.h"
using namespace std;
namespace odfaeg {
    namespace physic {
        /*Crée un cercle englobant, utilisé pour les détection de collisions, et la sélection
        * d'objets.
        */
        //Crée un cercle de centre center et de rayon radius.
        BoundingSphere::BoundingSphere() {
            center = math::Vec3f(0, 0, 0);
            radius = 1;
        }
        BoundingSphere::BoundingSphere (math::Vec3f center, float radius) {
            this->center = center;
            this->radius = radius;
        }
        bool BoundingSphere::onIntersects (BoundingVolume& interface) {
             return intersects(static_cast<BoundingSphere&>(interface));
        }
        bool BoundingSphere::intersects (BoundingSphere &other) {
            float d = center.computeDist(other.center);
            float rSum = radius + other.radius;
            return (d <= rSum);
        }
        bool BoundingSphere::intersects (BoundingEllipsoid &be) {
            return be.intersects(*this);
        }
        bool BoundingSphere::intersects (BoundingBox &bx) {
             math::Ray ray(center, bx.getCenter());
             math::Vec3f near, far;
             if (!intersectsWhere(ray, near, far)) {
                return false;
             }
             math::Vec3f d = far - center;
             return radius * radius + d.magnSquared() < center.computeDistSquared(bx.getCenter());
        }

        bool BoundingSphere::intersects (OrientedBoundingBox &obx) {
             math::Ray ray(center, obx.getCenter());
             math::Vec3f near, far;
             if (!intersectsWhere(ray, near, far)) {
                return false;
             }
             math::Vec3f d = far - center;
             return radius * radius + d.magnSquared() < center.computeDistSquared(obx.getCenter());
        }
        bool BoundingSphere::intersects (BoundingPolyhedron &bp) {
             math::Ray ray(center, bp.getCenter());
             math::Vec3f near, far;
             if (!intersectsWhere(ray, near, far)) {
                return false;
             }
             math::Vec3f d = far - center;
             return radius * radius + d.magnSquared() < center.computeDistSquared(bp.getCenter());
        }
        bool BoundingSphere::intersects (math::Ray &ray) {
            if (isPointInside(ray.getOrig()))
                return true;
            math::Vec3f d1 = center - ray.getOrig();
            float dp = d1.dot2(ray.getDir());
            if (dp < 0)
                return false;
            math::Vec3f p = ray.getOrig() + ray.getDir().normalize() * d1.projOnAxis(ray.getDir());
            math::Vec3f d2 = p - center;
            return d2.magnSquared() <= radius * radius;
        }
        bool BoundingSphere::intersectsWhere (math::Ray& r, math::Vec3f& near, math::Vec3f &far) {
            math::Vec3f p = r.getOrig();
            math::Vec3f d = r.getDir();
            math::Vec3f c = center;
             // this is the vector from p to c
            math::Vec3f vpc = c - p;
            if (vpc.dot2(d) < 0) {
                // when the sphere is behind the origin p
                // note that this case may be dismissed if it is
                // considered that p is outside the sphere
                if (vpc.magnSquared() > radius * radius) {
                    return false; // there is no intersection
                } else if (vpc.magnSquared() == radius * radius) {
                    near = far = p;
                    return true;
                } else {
                    // occurs when p is inside the sphere
                    math::Vec3f pc = p + d.normalize() * c.projOnAxis(d);
                    float dist = math::Math::sqrt(radius * radius - pc.computeDistSquared(c));
                    float di1 = dist - pc.computeDist(p);
                    near = p;
                    far = p + d * di1;
                    return true;
                }
            } else {
                // center of sphere projects on the ray
                math::Vec3f pc = p + d.normalize() * c.projOnAxis(d);
                if (c.computeDistSquared(pc) > radius * radius) {
                    // there is no intersection
                    return false;
                } else {
                    // distance from pc to i1 and to i2.
                    float dist = math::Math::sqrt((radius * radius) - pc.computeDistSquared(c));
                    float di1, di2;
                    if(vpc.magnSquared() > radius * radius) {
                        di1 = pc.computeDist(p) - dist;
                        near = p + d * di1;
                        di2 = pc.computeDist(p) + dist;
                        far = p + d * di2;
                    } else {
                        // origin is inside the sphere
                        di1 = pc.computeDist(p) + dist;
                        near = p;
                        far = p + d * di1;
                    }
                    return true;
                }
            }
        }
        //Test si un point est à l'intérieur du cercle.
        bool BoundingSphere::isPointInside (math::Vec3f &point) {
            float d = center.computeDist(point);
            return d <= radius;
        }
        math::Vec3f BoundingSphere::getPosition() {
            return math::Vec3f(center.x - radius, center.y - radius, center.z - radius);
        }
        math::Vec3f BoundingSphere::getCenter() {
            return center;
        }
        math::Vec3f BoundingSphere::getSize() {
            return math::Vec3f(radius * 2, radius * 2, radius * 2);
        }
        float BoundingSphere::getRadius () {
            return radius;
        }
        void BoundingSphere::scale(float s) {
            radius *= s;
        }
        void BoundingSphere::move(math::Vec3f t) {
            center += t;
        }
    }
}

