#include "../../../include/odfaeg/Physics/boundingEllipsoid.h"
#include "../../../include/odfaeg/Physics/boundingSphere.h"
#include "../../../include/odfaeg/Physics/boundingBox.h"
#include "../../../include/odfaeg/Physics/orientedBoundingBox.h"
#include "../../../include/odfaeg/Physics/boundingPolyhedron.h"
#include "../../../include/odfaeg/Graphics/transformMatrix.h"
namespace odfaeg {
    namespace physic {
        BoundingEllipsoid::BoundingEllipsoid(math::Vec3f center, int a, int b, int c) {
            this->center = center;
            radius = math::Vec3f(a, b, c);
            graphic::TransformMatrix tm;
            tm.setOrigin(center);
            tm.setTranslation(center);
            tm.update();
            translation = tm.getMatrix();
            tm.reset3D();
            tm.setOrigin(center);
            tm.setScale(radius);
            tm.update();
            scale = tm.getMatrix();
        }
        bool BoundingEllipsoid::intersects(BoundingSphere &bs) {
            //We do the same for check the collision with a circle, except taht the circle have only 1 ray.
            //The equation for the circle is then a bit different.
            math::Ray r(center, bs.getCenter());
            math::Vec3f near, far;
            if (!intersectsWhere(r, near, far))
                return false;
            math::Vec3f d = far - bs.getCenter();
            return d.magnSquared() <= bs.getRadius() * bs.getRadius();
        }
        bool BoundingEllipsoid::intersects(BoundingEllipsoid &be) {
             //The distance between the 2 centers.
             math::Ray r1(center, be.getCenter());
             math::Ray r2(be.getCenter(), center);
             math::Vec3f near1, near2, far1, far2;
             if (!intersectsWhere(r1, near1, far1))
                return false;
             if (!be.intersectsWhere(r2, near2, far2))
                return false;
             math::Vec3f d1 = far1 - center;
             math::Vec3f d2 = far2 - be.getCenter();
             return center.computeDistSquared(be.getCenter()) <= d1.magnSquared() + d2.magnSquared();
        }
        bool BoundingEllipsoid::intersects(BoundingBox &bx) {
             math::Ray r1(center, bx.getCenter());
             math::Ray r2(bx.getCenter(), center);
             math::Vec3f near1, near2, far1, far2;
             if (!intersectsWhere(r1, near1, far1))
                return false;
             if (!bx.intersectsWhere(r2, near2, far2))
                return false;
             math::Vec3f d1 = far1 - center;
             math::Vec3f d2 = far2 - bx.getCenter();
             return center.computeDistSquared(bx.getCenter()) <= d1.magnSquared() + d2.magnSquared();
        }
        bool BoundingEllipsoid::intersects(OrientedBoundingBox &obx) {
             //The line between the two centers.
             math::Ray r1(center, obx.getCenter());
             math::Ray r2(obx.getCenter(), center);
             math::Vec3f near1, near2, far1, far2;
             if (!intersectsWhere(r1, near1, far1))
                return false;
             if (!obx.intersectsWhere(r2, near2, far2))
                return false;
             math::Vec3f d1 = far1 - center;
             math::Vec3f d2 = far2 - obx.getCenter();
             return center.computeDistSquared(obx.getCenter()) <= d1.magnSquared() + d2.magnSquared();
        }
        bool BoundingEllipsoid::intersects(BoundingPolyhedron &bp) {
             math::Ray r1(center, bp.getCenter());
             math::Ray r2(bp.getCenter(), center);
             math::Vec3f near1, near2, far1, far2;
             if (!intersectsWhere(r1, near1, far1))
                return false;
             if (!bp.intersectsWhere(r2, near2, far2))
                return false;
             math::Vec3f d1 = far1 - center;
             math::Vec3f d2 = far2 - bp.getCenter();
             return center.computeDistSquared(bp.getCenter()) <= d1.magnSquared() + d2.magnSquared();
        }
        bool BoundingEllipsoid::intersects (math::Ray &r) {
            math::Matrix4f transform = scale * rotation * translation;
            math::Matrix4f invTransform = transform.inverse();
            math::Vec3f orig = invTransform * r.getOrig();
            math::Vec3f ext = invTransform * r.getExt();
            math::Ray tRay (orig, ext);
            BoundingSphere bs(math::Vec3f(0, 0, 0), 1);
            if (!bs.intersects(tRay))
                return false;
            return false;
        }
        bool BoundingEllipsoid::intersectsWhere(math::Ray& r, math::Vec3f& near, math::Vec3f& far) {
            math::Matrix4f transform = scale * rotation * translation;
            math::Matrix4f invTransform = transform.inverse();
            math::Vec3f orig = invTransform * r.getOrig();
            math::Vec3f ext = invTransform * r.getExt();
            math::Ray tRay (orig, ext);
            math::Vec3f i1, i2;
            BoundingSphere bs(math::Vec3f(0, 0, 0), 1);
            if (!bs.intersectsWhere(tRay, i1, i2))
                return false;
            near = transform * i1;
            far = transform * i2;
            return true;
        }
        bool BoundingEllipsoid::isPointInside (math::Vec3f point) {
            math::Ray r (center, point);
            if (!intersects(r))
                return false;
            return true;
        }
        math::Vec3f BoundingEllipsoid::getPosition() {
            return math::Vec3f (center.x - radius.x, center.y - radius.y, center.z - radius.z);
        }
        math::Vec3f BoundingEllipsoid::getCenter() {
            return center;
        }
        math::Vec3f BoundingEllipsoid::getRadius() {
            return radius;
        }
        math::Vec3f BoundingEllipsoid::getSize() {
            return math::Vec3f(radius.x * 2, radius.y * 2, radius.z * 2);
        }
        void BoundingEllipsoid::move(math::Vec3f t) {
            center += t;
            graphic::TransformMatrix tm;
            tm.setOrigin(center);
            tm.setTranslation(center);
            tm.update();
            translation = tm.getMatrix();
        }
        void BoundingEllipsoid::setScale(math::Vec3f s) {
            graphic::TransformMatrix tm;
            tm.setOrigin(center);
            tm.setScale(s);
            tm.update();
            scale = tm.getMatrix();
        }
        void BoundingEllipsoid::rotate(float angle, math::Vec3f axis) {
            graphic::TransformMatrix tm;
            tm.setOrigin(center);
            tm.setRotation(axis, angle);
            tm.update();
            rotation = tm.getMatrix();
        }
    }
}
