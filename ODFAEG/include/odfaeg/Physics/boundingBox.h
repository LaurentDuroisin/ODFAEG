#ifndef ODFAEG_BOUNDING_BOX_HPP
#define ODFAEG_BOUNDING_BOX_HPP
#include "../Math/vec2f.h"
#include "../Math/ray.h"
#include <climits>
#include "boundingVolume.h"
#include "../Math/computer.h"
/**
  *\namespace odfaeg
  * the namespace of the Opensource Development Framework Adapted for Every Games.
  */
namespace odfaeg {
    namespace graphic {
        class TransformMatrix;
    }
    namespace physic {
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
                bool onIntersects (BaseInterface& interface) {
                    return interface.intersects(*this);
                }
                bool onIntersects (BaseInterface& interface, math::Ray& ray, bool segment) {
                    return interface.intersects(ray, segment);
                }
                bool onIntersects (BaseInterface& interface, math::Ray& ray, math::Vec3f& near, math::Vec3f& far) {
                    return interface.intersectsWhere(ray, near, far);
                }
                bool intersects(BoundingVolume &bv);
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
                bool intersects (math::Ray& ray, bool segment);
                bool intersectsWhere (math::Ray &ray, math::Vec3f& i1, math::Vec3f& i2);
                /** \fn bool isPointInside (Vec2f point)
                *   \brief test if a point is in the bounding box.
                *   \param the point to test in.
                *   \return the result of the collision test.
                */
                bool isPointInside (math::Vec3f point);
                /**\fn Vec2f getCenter()
                *  \brief gives the center of the bounding box.
                *  \return the center of the bounding box.
                */
                math::Vec3f getCenter();
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
                math::Vec3f getSize();
                math::Vec3f getPosition();
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
                void move (math::Vec3f t);
                void scale (math::Vec3f s);
                const BoundingBox& operator= (const BoundingBox& other) {
                    x = other.x;
                    y = other.y;
                    z = other.z;
                    width = other.width;
                    height = other.height;
                    depth = other.depth;
                    points = other.points;
                    return *this;
                }
                template <typename Archive>
                void vtserialize(Archive & ar) {
                    BoundingVolume::vtserialize(ar);
                    ar(x);
                    ar(y);
                    ar(z);
                    ar(width);
                    ar(height);
                    ar(depth);
                    ar(center);
                }
                BoundingBox transform(graphic::TransformMatrix &tm);
                std::unique_ptr<BoundingVolume> clone() {
                    return std::make_unique<BoundingBox>(*this);
                }
                std::array<math::Vec3f, 8> getVertices();
            private :
                std::array<math::Vec3f, 8> points;
                int x, y, z, width, height, depth; /**< the x position of the bounding box */
                /**< the y position of the bounding box */
                /**< the z position of the bounding box */
                /**< the width of the bounding box */
                /**< the height of the bounding box */
                /**< the depth of the bounding box */
                math::Vec3f center;
                /**< the center of the bounding box */
        };
    }
}
#endif
