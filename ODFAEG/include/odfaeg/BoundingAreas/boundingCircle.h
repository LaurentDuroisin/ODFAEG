#ifndef ODFAEG_BOUNDING_SPHERE_HPP
#define ODFAEG_BOUNDING_SPHERE_HPP
#include "../Math/vec2f.h"
#include "segment.h"
#include "boundingAreas.h"
/**
  *\namespace odfaeg
  * the namespace of the Opensource Development Framework Adapted for Every Games.
  */
namespace odfaeg {
/**
  * \file boundingSphere.h
  * \class BoudingSphere
  * \brief Manage a bounding sphere for collision detection
  * \author Duroisin.L
  * \version 1.0
  * \date 1/02/2014
  *
  * Manage a bounding sphere for collision detection.
  *
  */
class BoundingEllipsoid;
class OrientedBoundingBox;
class BoundingPolyhedron;
class BoundingBox;
class BoundingSphere : public BoundingVolume {
    public :
        /**
        * \fn BoundingSphere (Vec2f center, float radius)
        * \brief Constructor : create an instance of a bounding sphere.
        * \param the center of the bounding sphere.
        * \param the radius of the bounding sphere.
        */
        BoundingSphere (Vec3f center, float radius);
        /**\fn bool intersects (BoundingSphere &other)
         * \brief check if the bounding sphere is in collision with an other.
         * \param the other bounding sphere to test with.
         * \return the result of the collision test.
         */
        bool intersects (BoundingSphere &other);
        /** \fn bool intersects (BoundingEllipsoid &be)
         *  \brief check if the bounding sphere is in collision with a bounding ellipsoid.
         *  \param the bouding ellipsoid to test with.
         *  \return the result of the collision test.
         */
        bool intersects (BoundingEllipsoid &be);
        /** \fn bool intersects (BoundingBox &br)
         *  \brief check if the bounding sphere is in collision with a bounding box.
         *  \param the bounding box to test with.
         *  \return the result of the collision test.
         */
        bool intersects (BoundingBox &obr);
        /**\fn bool intersects (OrientedBoundingRectangle &obr)
        *  \brief check if the bounding sphere is in collision with an oriented bounding box.
        *  \param the oriented bounding box to test with.
        *  \return the result of the collision test.
        */
        bool intersects (OrientedBoundingBox &obr);
        /**\fn bool intersects (BoundingPolygon &bp)
        *  \brief check if the bounding sphere is in collision with a bounding polyhedron.
        *  \param the bounding polyhedron to test with.
        *  \return the result of the collision test.
        */
        bool intersects (BoundingPolyhedron &bp);
        /** \fn bool isPointInside (Vec2f &point)
         *  \brief check if a point is inside the sphere.
         *  \param the point to test in.
         *  \return the result of the collision test.
         */
        bool isPointInside (Vec3f &point);
        /** \fn bool intersects (Segment &ray)
         *  \brief check if a ray interects the sphere.
         *  \param the ray to test with.
         *  \return the result of the collision test.
         */
        bool intersects (Ray& ray);
        /** \fn bool intersects (Segment &ray, Vec2f &near, Vec2f &far)
         *  \brief check if a ray interects the sphere and get the points of the intersection.
         *  \param the ray to test with.
         *  \param the vector to store the nearest point of the intersection. (from the ray's origin)
         *  \param the vector to store the farest point of the intersection. (from the ray"s origin)
         *  \return the result of the collision test.
         */
        bool intersectsWhere (Ray &ray, Vec3f &near, Vec3f &far);
        /** \fn Vec2f getCenter()
        *   \return the center of the bounding sphere.
        */
        Vec3f getCenter();
        /** \fn Vec2f getRadius()
        *   \return the radius of the bounding sphere.
        */
        float getRadius();
    private :
        Vec3f center; /** < the center of the bouding sphere */
        float radius; /** < the radius of the bouding sphere */
};
}
#endif
