#ifndef TRANSFORM_MATRIX
#define TRANSFORM_MATRIX

#include "vec4.h"
#include "vec2f.h"
#include "matrix4.h"
#include "matrix3.h"
#include "maths.h"
#include "../BoundingAreas/axisAlignedBoundingBox.h"
#include "../BoundingAreas/boundingRectangle.h"
#include <SFML/Graphics.hpp>
/**
  *\namespace odfaeg
  * the namespace of the Opensource Development Framework Adapted for Every Games.
  */
namespace odfaeg
{
    namespace math {
        /**
          * \file transformMatrix.h
          * \class TransformMatrix
          * \brief Manage a transformation matrix.
          * \author Duroisin.L
          * \version 1.0
          * \date 1/02/2014
          *
          * Manage transformations matrix who are used to perform 2D and 3D transformations.
          * The construction of the transformations matrix depend on the translation, scale, rotation and origin provided.
          * The matrix is constructed only when it's necessary.
          */
        class TransformMatrix : public Serializer<TransformMatrix>
        {
        private :
            Matrix4f matrix4f; /**< the 3D transformation matrix*/
            Matrix3f matrix3f; /**< the 2D transformation matrix*/
            Matrix4f invMat4f; /**< the 3D transformation's inverse matrix*/
            Matrix3f invMat3f; /**< the 2D transformation's inverse matrix*/
            Vec3f t3d; /**< the 3D translation*/
            Vec2f t2d; /**< the 2D translation*/
            Vec3f s3d; /**< the 3D scale*/
            Vec2f s2d; /**< the 2D scale*/
            float r3d; /**< the 3D rotation angle*/
            Vec3f r3dAxis; /**< the 3D rotation axis (the rotations'll be done around this axis)*/
            Vec2f o2d; /**< the 2D origin*/
            Vec3f o3d; /**< the 3D origin*/
            float r2d; /**< the 2D rotation angle*/
            bool needToUpdate2D; /** < determine if the 2D transformation matrix have to be updated before the drawing*/
            bool inverseNeedToUpdate2D; /** < determine if the 2D transformation's inverse matrix have to be updated before the drawing*/
            bool needToUpdate3D; /** < determine if the 3D transformation matrix have to be updated before the drawing*/
            bool inverseNeedToUpdate3D; /** < determine if the 3D transformation matrix have to be updated before the drawing*/
        public :
            /**
            * \fn TransformMatrix ()
            * \brief Default constructor : set the identity transformation's matrix.
            */
            TransformMatrix();
            /**
            * \fn void reset2D()
            * \brief Reset the 2D transformation matrix to the identity transformation's matrix.
            */
            void reset2D ();
            /**
            * \fn void reset3D()
            * \brief Reset the 3D transformation matrix to the identity transformation's matrix.
            */
            void reset3D ();
            /**
            * \fn sf::Transform getSFMLTransform()
            * \brief return an sf::Transform object to pass the 2D transformation matrix to the SFML transformation matrix.
            * \return an sf::Transform object.
            */
            sf::Transform getSFMLTransform();
            /**
            * \fn void setTranslation (const Vec3f trans)
            * \brief set a 3D translation.
            * \param the 3D translation.
            */
            void setTranslation (const Vec3f trans);
            /**
            * \fn void setScale (const Vec3f scale)
            * \brief set a 3D scale.
            * \param the 3D scale.
            */
            void setScale (const Vec3f scale);
            /**
            * \fn void setScale (float sx, float sy)
            * \brief set the 2D scale.
            * \param the 2D scale.
            */
            void setScale (float sx, float sy);
            /**
            * \fn void setTranslation (int dx, int dy)
            * \brief set the 2D translation.
            * \param the 2D translation.
            */
            void setTranslation (int dx, int dy);
            /**
            * \fn void setRotation (const Vec3f axis, float teta)
            * \brief set the 3D rotation axis.
            *
            *        / <- 3D rotation axis.
            *      (/) <- rotation angle.
            *      /  . .
            *     /  . .  <- entity's vertices.
            *    /   . .
            *   O <- origin.
            * \param axis : the 3D rotation axis.
            * \param teta : the rotation angle.
            */
            void setRotation (const Vec3f axis, float teta);
            /**
            * \fn void setRotation (int angle)
            * \brief set the angle for the 2D rotation.
            * \param the rotation angle.
            */
            void setRotation (int angle);
            /**
            * \fn setOrigin (float ox, float oy)
            * \brief set the coordinates of the 2D origin.
            * \param ox : the x coordinate of the origin
            * \param oy : the y coordinate of the origin
            */
            void setOrigin(float ox, float oy);
            /**
            * \fn setOrigin (Vec3f origin)
            * \brief set the coordinates of the 3D origin.
            * \param the 3D origin's position.
            */
            void setOrigin(Vec3f origin);
            /**
            * \fn Vec3f transform (const Vec3f vec3)
            * \brief transform a 3D point and give the resulting point
            * \param the point to be transformed.
            * \return the resulting point.
            */
            Vec3f transform (const Vec3f vec3);
            /**
            * \fn Vec2f transform (const Vec2f vec2)
            * \brief transform a point to an another point.
            * \param the point to be transformed.
            * \return the resulting point.
            */
            Vec2f transform (const Vec2f vec2);
            /**
            * \fn void combine (Matrix3f matrix)
            * \brief combine the transformation matrix with another transformatin on matrix.
            * \param the transformation matrix to be combined with.
            */
            void combine(Matrix3f matrix3f);
            /**
            * \fn Vec3f inverseTransform (const Vec3f)
            * \brief untransform a point and give the resulting point.
            * \param the point to be transformed.
            * \return the resulting point.
            */
            Vec3f inverseTransform (const Vec3f vec3);
            /**
            * \fn Vec2f inverseTransform (const Vec2f)
            * \brief untransform a point and give the resulting point.
            * \param the point to be untransformed.
            * \return the resulting point.
            */
            Vec2f inverseTransform (const Vec2f vec2);
            /**
            *   \fn Matrix3f get2DMatrix() const
            *   \brief return the 2D transformation matrix
            *   \return the 2D transformation matrix.
            */
            Matrix3f get2DMatrix () const ;
            /**
            *   \fn Matrix4f get2DMatrix() const
            *   \brief return the 3D transformation matrix
            *   \return the 3D transformation matrix.
            */
            Matrix4f get3DMatrix ();
            /**
            *   \fn float* getGlMatrix()
            *   \brief return an array of the transformation matrix elements to pass it to opengl and
            *   construct the matrix again if it's necessary.
            */
            float* getGlMatrix();
            /**
            *  \fn AABB transformAABB(AABB aabb)
            *  \brief recompute the englobing AABB when an entity is transformed.
            *  \param the current AABB of the entity.
            *  \return the resulting AABB.
            */
            AABB transformAABB (AABB aabb);
            /**
            *  \fn BoundingRectangle transformRect(BoundingRectangle)
            *  \brief recompute the bounding rectangle when an entity is transformed.
            *  \return the resulting bounding rectangle.
            */
            Bounding transformRect (BoundingRectangle rect);
            bool operator== (const TransformMatrix& other);
            template <typename Archive>
            void serialize (Archive &ar) {
                ar | matrix4f;
                ar | t3d;
                ar | s3d;
                ar | o3d;
                ar | r3dAxis;
                ar & r3d;
            }
        };
    }
}
#endif


