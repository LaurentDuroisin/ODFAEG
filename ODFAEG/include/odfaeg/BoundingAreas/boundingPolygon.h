#ifndef ODFAEG_BOUNDING_POLYHEDRON_HPP
#define ODFAEG_BOUNDING_POLYHEDRON_HPP
#include <vector>
#include "../Math/vec2f.h"
#include "../Math/vec4.h"
#include "boundingRectangle.h"
/**
  *\namespace odfaeg
  * the namespace of the Opensource Development Framework Adapted for Every Games.
  */
namespace odfaeg {
/**
  * \file boundingPolyhedron.h
  * \class BoudingPolyhedron
  * \brief Manage a bounding polyhedron for collision detection
  * \author Duroisin.L
  * \version 1.0
  * \date 1/02/2014
  *
  * Manage a bounding polyhedron for collision detection.
  *
  */
class BoundingSpere;
class BoundingBox;
class OrientedBoundingBox;
class BoundingPolyhedron : public BoundingVolume {
    public :
    /**\fn BoundingPolyhedron()
    * \brief Default constructor : construct a bounding polyhedron. (with no points)
    */
    BoundingPolyhedron ();
     /**\fn ~BoundingPolyhedron()
    * \brief destructor : delete all the points of the bounding polyhedron.
    */
    ~BoundingPolyhedron();
    /**\fn void addPoint(Vec3f *p)
    *  \param add a point to a polyhedron.
    */
    void addPoint(Vec3f *p);
    /**\fn Vec2f getPoint(unsigned int index)
    *  \brief get the point of the polyhedron with the given index.
    *(0 = the index of the first point added, 1 = the index of the second, etc...
    * The index must be between 0 and the number of polyhedron's points - 1.
    *  \param the index of the point
    *  \return the point of the polyhedron at the given index.
    */
    Vec3f getPoint (unsigned int index);
    /**\fn bool intersects (BoundingSphere &bs)
    *  \brief check if the bounding polyhedron collides with the bounding sphere.
    *  \param the bounding sphere to test with.
    *  \return the result of the collision test.
    */
    bool intersects (BoundingSphere &bs);
    /**\fn bool intersects (BoundingEllipsoid &be)
    *  \brief check if the bounding polyhedron collides with the bounding ellipsoid.
    *  \param the bounding ellipsoid to test with.
    *  \return the result of the collision test.
    */
    bool intersects (BoundingEllipsoid &be);
    /**\fn bool intersects (BoundingBox &bx)
    *  \brief check if the bounding polyhedron collides with the bounding box.
    *  \param the bounding box to test with.
    *  \return the result of the collision test.
    */
    bool intersects (BoundingBox &bx);
    /**\fn bool intersects (OrientedBoundingBox &obx)
    *  \brief check if the bounding polyhedron collides with the oriented bounding box.
    *  \param the oriented bounding box to test with.
    *  \return the result of the collision test.
    */
    bool intersects (OrientedBoundingBox &obx);
    /**\fn bool intersects (BoundingPolyhedron &bp)
    *  \brief check if the bounding polyhedron collides with an other.
    *  \param the other bounding polyhedron to test with.
    *  \return the result of the collision test.
    */
    bool intersects (BoundingPolyhedron &bp);
    /**\fn bool isPointInside (Vec3f &point)
    *  \brief check if a given point is in the bounding polyhedron.
    *  \param the point to test in.
    *  \return the result of the collision test.
    */
    bool isPointInside (Vec3f &point);
    /**\fn bool operator== (const BoundingPolyhedron &bp)
    *  \brief check if two bounding polyhedrons are equals.
    *  \param the other bounding polyhedron to test with.
    *  \return the result of the comparaison.
    */
    bool operator== (const BoundingPolyhedron &bs);
    /**\fn Vec2f getCenter()
    *  \brief return the center of the bounding polyhedron
    *  \return the center of the bounding polyhedron.
    */
    Vec3f getCenter();
    /**\fn int nbPoints()
    *  \brief return the number of points of the bounding polyhedron
    *  \return the number of points of the bounding polyhedron.
    */
    int nbPoints ();

    private :
    /**\fn void computeCenter()
    *  \brief recompute the center of the bounding polyhedron. (When we add or remove points.)
    *
    */
    void computeCenter ();
    std::vector<Vec3f*> points; /**< The points of the bounding polyhedron.*/
    Vec3f center; /**< The center of the bounding polyhedron.*/
};
}
#endif
