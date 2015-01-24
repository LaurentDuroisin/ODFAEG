#ifndef ODFAEG_BOUNDING_BOX_HPP
#define ODFAEG_BOUNDING_BOX_HPP
#include "../Math/vec2f.h"
#include "segment.h"
#include <climits>
#include "boundingAreas.h"
/**
  *\namespace odfaeg
  * the namespace of the Opensource Development Framework Adapted for Every Games.
  */
namespace odfaeg {
class BoundingSphere;
class BoundingEllipsoid;
class OrientedBoundingBox;
class BoundingPolyhedron;
/**
  * \file boundingBox.h
  * \class BoudingBox
  * \brief Manage a bounding box for collision detection
  * \author Duroisin.L
  * \version 1.0
  * \date 1/02/2014
  *
  * Manage a bounding box for collision detection.
  * The bounding box is aligned with the x and y axis.
  *
  */
class BoundingBox : public BoundingVolume {
    public :
        /** \fn BoundingBox()
        * \brief Default constructor (initialize a bounding retangle at position (0, 0) and with a size of (0, 0).
        */
        BoundingBox ();
        /** \fn BoundingBox (int, int, int, int  int, int)
         * \brief Initialize a bounding box with the given position and the given size
         * \param the x position of the bounding box
         * \param the y position of the bounding box
         * \param the z position of the bounding box
         * \param the width of the bounding box
         * \param the height of the bounding box
         * \param the depth of the bounding box
         */
        BoundingBox (int x, int y, int z, int width, int height, int depth);
        /** \fn bool intersects (BoundingSphere &bs)
        *   \brief test if a bounding sphere collides with the bounding sphere.
        *   \param the bounding sphere to test with.
        *   \return the result of the collision test.
        */
        bool intersects (BoundingSphere &bs);
        /** \fn bool intersects (BoundingEllipsoid &be)
        *   \brief test if a bounding ellipsoid collides with the bounding box.
        *   \param the bounding ellipsoid to test with.
        *   \return the result of the collision test.
        */
        bool intersects (BoundingEllipsoid &bc);
        /** \fn bool intersects (BoundingBox &br)
        *   \brief test if an other bounding box collides with the bounding box.
        *   \param the other bounding box to test with.
        *   \return the result of the collision test.
        */
        bool intersects (BoundingBox &bx);
        /** \fn bool intersects (OrientedBoundingBox &br)
        *   \brief test if an oriented bounding sphere collides with the bounding box.
        *   \param the oriented bounding box to test with.
        *   \return the result of the collision test.
        */
        bool intersects (OrientedBoundingBox &obx);
        /** \fn bool intersects (BoundingPolyhedron &bp)
        *   \brief test if a bounding polyhedron collides with the bounding box.
        *   \param the bounding polyhedron to test with.
        *   \return the result of the collision test.
        */
        bool intersects (BoundingPolyhedron &bp);
        /** \fn bool intersects (Ray &ray)
        *   \brief test if a ray collides with the bounding box.
        *   \param the Segment to test with.
        *   \return the result of the collision test.
        */
        bool intersects (Ray& ray);
        /** \fn bool isPointInside (Vec2f point)
        *   \brief test if a point is in the bounding box.
        *   \param the point to test in.
        *   \return the result of the collision test.
        */
        bool isPointInside (Vec3f point);
        /**\fn Vec2f getCenter()
        *  \brief gives the center of the bounding box.
        *  \return the center of the bounding box.
        */
        Vec3f getCenter();
       /**\fn float getWidth()
        *  \brief gives the width of the bounding box.
        *  \return the width of the bounding box.
        */
        float getWidth();
        /**\fn float getHeight()
        *  \brief gives the height of the bounding box.
        *  \return the height of the bounding box.
        */
        float getHeight();
        /**\fn float getDepth()
        *  \brief gives the depth of the bounding box.
        *  \return the depth of the bounding box.
        */
        float getDepth();
        /**\fn Vec3f getPosition()
        *  \brief gives the position of the bounding box.
        *  \return the position of the bounding box.
        */
        Vec3f getPosition();
        /**\fn void setPosition(int x, int y, int z)
        *  \brief set the position of the bounding box.
        *  \param the x position of the bounding box.
        *  \param the y position of the bounding box.
        */
        void setPosition(int x, int y, int z);
        /**\fn void setSize(int width, int height, int depth)
        *  \brief set the size of the bounding box.
        *  \param the width of the bounding box.
        *  \param the height of the bounding box.
        *  \param the depth of the bounding box.
        */
        void setSize(int width, int height, int depth);
    private :
        int x, y, z, width, height, depth; /**< the x position of the bounding box */
        /**< the y position of the bounding box */
        /**< the z position of the bounding box */
        /**< the width of the bounding box */
        /**< the height of the bounding box */
        /**< the depth of the bounding box */
        Vec3f center;
        /**< the center of the bounding box */
};
}
#endif
