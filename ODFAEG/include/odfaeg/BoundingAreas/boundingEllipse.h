#ifndef ODFAEG_BOUNDING_ELLIPSOID_HPP
#define ODFAEG_BOUNDING_ELLIPSOID_HPP
#include "../Math/vec2f.h"
#include "segment.h"
#include "boundingAreas.h"
/**
  *\namespace odfaeg
  * the namespace of the Opensource Development Framework Adapted for Every Games.
  */
namespace odfaeg {
/**
 *\file boundingEllipsoid.h
 *\class BoundingEllipsoid
 *\brief manage a bounding  ellipsoid for collision detection.
 *\author Duroisin.L
 *\version 1.0
 *\date 1/02/2014
 *
 * Manage a bounding  ellipsoid for collision detection.
 *
 */
class BoundingSphere;
class BoundingBox;
class OrientedBoundingBox;
class BoundingPolyhedron;
class BoundingEllipsoid : public BoundingVolume {
    public :
        /**
        *\fn BoudingEllipsoid (Vec2f center, int a, int b, int c)
        *\brief Constructor of the class BoundingEllipsoid.
        *\param the center of the bounding ellipsoid.
        *\param the x radius of the bounding ellipsoid.
        *\param the y radius of the bounding ellipsoid.
        *\param the z radius of the bounding ellipsoid.
        */
        BoundingEllipsoid (Vec3f center, int a, int b, int c);
        /**
        *\fn bool intersects (BoundingSphere &bc)
        *\brief check if the bounding ellipsoid is in collision with a bounding sphere.
        *\param the bounding sphere to test with.
        */
        bool intersects (BoundingSphere &bs);
        /**
        *\fn bool intersects (BoundingEllipsoid &be)
        *\brief check if the bounding ellipsoid is in collision with another.
        *\param the other bounding  ellispsoid to test with.
        */
        bool intersects (BoundingEllipsoid &be);
        /**
        *\fn bool intersects (BoundingBox &bx)
        *\brief check if the bounding ellipsoid is in collision with a bounding box.
        *\param the bounding box to test with.
        */
        bool intersects (BoundingBox &bx);
        /**
        *\fn bool intersects (OrientedBoundingBox &bx)
        *\brief check if the bounding ellipsoid is in collision with an oriented bounding box.
        *\param the oriented bounding box to test with.
        */
        bool intersects (OrientedBoundingBox &obx);
        /**
        *\fn bool intersects (BoundingPolyhedron &bp)
        *\brief check if the oriented bounding ellipsoid is in collision with a bounding polyhedron.
        *\param the bounding polyhedron to test with.
        */
        bool intersects (BoundingPolyhedron &bp);
        /**
        *\fn bool intersects (Ray &ray)
        *\brief check if the ray is in collision with another.
        *\param the ray to test with.
        */
        bool intersects (Ray& ray);
        /**
        *\fn bool intersects (Ray &ray)
        *\brief check if the ray is in collision with another and give the intersection points.
        *\param the ray to test with.
        */
        bool intersectsWhere (Ray& ray, Vec3f& near, Vec3f &far);
        /**
        *\fn bool intersects (Vec3f p)
        *\brief check if a point is in the bounding ellipsoid.
        *\param the point to test in.
        */
        bool isPointInside(Vec3f p);
        /**
        *\fn Vec3f getCenter ()
        *\brief give the center of the bounding ellispoid.
        *\return Vec3f the center of the bounding ellipsoid.
        **/
        Vec3f getCenter();
        /**
        *\fn Vec3f getRadius ()
        *\brief give the x, y and z radius of the bounding ellispoid.
        *\return Vec3f the radius of the bounding ellipsoid.
        **/
        Vec3f getRadius();

    private :
        Vec3f radius; /**< The x, y and z radius of the bounding ellipsoid.*/
        Vec3f center; /**< The center of the bounding ellipsoid.*/
};
}
#endif
