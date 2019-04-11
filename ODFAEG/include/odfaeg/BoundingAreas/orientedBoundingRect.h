#ifndef ODFAEG_ORIENTED_BOUNDING_BOX_HPP
#define ODFAEG_ORIENTED_BOUNDING_BOX_HPP
#include "../Math/vec2f.h"
#include "segment.h"
#include "boundingAreas.h"
#include "../Math/computer.h"
/**
  *\namespace odfaeg
  * the namespace of the Opensource Development Framework Adapted for Every Games.
  */
namespace odfaeg {
class BoundingCircle;
class BoundingPolygon;
class BoundingEllipse;
class BoundingRectangle;
/**
  * \file orientedBoundingBox.h
  * \class OrientedBoundingBox
  * \brief Manage an oriented bounding box for collision detection
  * \author Duroisin.L
  * \version 1.0
  * \date 1/02/2014
  *
  * Manage an oriented bounding box for collision detection.
  *
  */
class OrientedBoundingBox : public BoundingVolume {
    public :
        /**\fn OrientedBoundingBox (Vec3f p1, Vec3f p2, Vec3f p3, Vec3f p4, Vec3f p5, Vec3f p6, Vec3f p7, Vec3f p8)
         * \brief Constructor (instanciate an oriented bounding box with 8 points)
         * \param The first point of the oriented bounding box.
         * \param The second point of the oriented bounding box.
         * \param The third point of the oriented bounding box.
         * \param The fourth point of the oriented bounding box.
         * \param The fifth point of the oriented bounding box.
         * \param The sixth point of the oriented bounding box.
         * \param The seventh point of the oriented bounding box.
         * \param The heigthth point of the oriented bounding box.
         */
        OrientedBoundingBox (Vec3f p1, Vec3f p2, Vec3f p3, Vec3f p4, Vec3f p5, Vec3f p6, Vec3f p7, Vec3f p8);
        /**\fn bool intersects (BoundingSphere &bs)
        *  \brief check if the oriented bounding box collides with a bounding sphere.
        *  \param the bounding sphere to test with.
        *  \return the result of the collision test.
        */
        bool intersects (BoundingSphere &bs);
        /**\fn bool intersects (BoundingEllipsoid &be)
        *  \brief check if the oriented bounding box collides with a bounding ellipsoid.
        *  \param the bounding ellips to test with.
        *  \return the result of the collision test.
        */
        bool intersects (BoundingEllipsoid &be);
        /**\fn bool intersects (BoundingBox &br)
        *  \brief check if the oriented bounding box collides with a bounding box.
        *  \param the bounding box to test with.
        *  \return the result of the collision test.
        */
        bool intersects (BoundingBox &bx);
        /**\fn bool intersects (OrientedBoundingBox &other)
        *  \brief check if the oriented bounding box collides with another.
        *  \param the other oriented bounding box to test with.
        *  \return the result of the collision test.
        */
        bool intersects (OrientedBoundingBox &other);
        /**\fn bool intersects (BoundingPolyhedron &bp)
        *  \brief check if the oriented bounding box collides with a bounding polyhedrin.
        *  \param the bounding polyhedron to test with.
        *  \return the result of the collision test.
        */
        bool intersects (BoundingPolyhedron &bp);
        /**\fn bool intersects (Ray &ray)
        *  \brief check if the oriented bounding box collides with a segment.
        *  \param the bounding polygon to test with.
        *  \return the result of the collision test.
        */
        bool intersects (Ray &ray);
        /**\fn bool isPointInside (Vec3f point)
        *  \brief check if the point is inside the oriented bounding box.
        *  \param the point to test in.
        *  \return the result of the collision test.
        */
        bool isPointInside (Vec3f point);
        /**\fn void setCorners (Vec3f p1, Vec3f p2, Vec3f p3, Vec3f p4, Vec3f p5, Vec3f p6, Vec3f p7, Vec3f p8)
        * \brief set the 8 points of the oriented bounding box.
        * \param The first point of the oriented bounding box.
        * \param The second point of the oriented bounding box.
        * \param The third point of the oriented bounding box.
        * \param The fourth point of the oriented bounding box.
        * \param The fifth point of the oriented bounding box.
        * \param The sixth point of the oriented bounding box.
        * \param The seventh point of the oriented bounding box.
        * \param The heigthth point of the oriented bounding box.
        */
        void setCorners (Vec3f p1, Vec3f p2, Vec3f p3, Vec3f p4, Vec3f p5, Vec3f p6, Vec3f p7, Vec3f p8);
        /**\fn Vec2f getCenter()
        *  \return the center of the oriented bounding box.
        **/
        Vec3f getCenter();
        /**\fn int getWidth()
        *  \brief return the width of the oriented bounding box.
        *  \return the width of the oriented bounding box.
        */
        int getWidth();
        /**\fn int getHeight()
        *  \brief return the width of the oriented bounding box.
        *  \return the width of the oriented bounding box.
        */
        int getHeight();
        /**\fn int getDepth()
        *  \brief return the depth of the oriented bounding box.
        *  \return the depth of the oriented bounding box.
        */
        int getDepth();
        /**\fn Vec3f getBX()
        *  \brief return the first edge of the oriented bounding box.
        *  \return the first edge of the oriented bounding box.
        */
        Vec3f getBX();
        /**\fn Vec3f getBY()
        *  \brief return the second edge of the oriented bounding box.
        *  \return the second edge of the oriented bounding box.
        */
        Vec3f getBY();
        /**\fn Vec3f getBZ()
        *  \brief return the second edge of the oriented bounding box.
        *  \return the second edge of the oriented bounding box.
        */
        Vec3f getBZ();
        /**\fn Vec3f getPosition()
        *  \brief return the position of the oriented bounding box
        *  \return the position of the oriented bounding box.
        */
        Vec3f getPosition();
        /**\fn Vec3f getCenter()
        *  \brief return the center of the oriented bounding box
        *  \return the center of the oriented bounding box.
        */

    private :
        int x, y, z, width, height, depth; /**< the x position of the bounding box*/
        /**< the y position of the bounding box*/
        /**< the z position of the bounding box*/
        /**< the width of the bounding box*/
        /**< the height of the bounding box*/
        /**< the depth position of the bounding box*/
        Vec3f bx, by, bz; /**< the first edge of the bounding box*/
        /**< the second edge of the bounding box*/
        /**< the third edge of the bounding box*/
        Vec3f center; /**< the center of the bounding box*/
};
}

#endif // ORIENTED_BOUNDING_RECT
