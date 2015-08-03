#ifndef ODFAEG_BASE_CHANGEMENT_MATRIX
#define ODFAEG_BASE_CHANGEMENT_MATRIX
#include "../Math/matrix2.h"
#include "../Math/vec2f.h"
#include "export.hpp"
/**
  *\namespace odfaeg
  * the namespace of the Opensource Development Framework Adapted for Every Games.
  */
namespace odfaeg {
    namespace graphic {
        /**
          * \file baseChangementMatrix.h
          * \class BaseChangementMatrix
          * \brief Represent a matrix to pass from a landmark to another one.
          * \author Duroisin.L
          * \version 1.0
          * \date 1/02/2014
          *  Represent a matrix to pass from a landmark to another one.
          */
        class ODFAEG_GRAPHICS_API BaseChangementMatrix {
        public :
            /** \fn void set2DIsoMatrix();
            *   \brief set the 2D isometric base changement matrix.
            */
            void set2DIsoMatrix();
            /**
            *  \fn Vec3f changeOfBase(Vec3f v);
            *  \brief convert the given point into the landmark.
            *  \param v : the given point.
            *  \return the coordinates of the point in the new landmark.
            */
            math::Vec2f changeOfBase(math::Vec2f v);
            /**
            *  \fn Vec3f unchangeOfBase(Vec3f v);
            *  \brief convert the given point of the landmark into the original landmark.
            *  \param v : the given point of the landmark.
            *  \return the coordinates of the point in the original landmark.
            */
            math::Vec2f unchangeOfBase(math::Vec2f v);
            /** \fn Matrix4f getMatrix()
            *   \brief get the base changement matrix.
            *    \return the base changement matrix.
            */
            math::Matrix2f getMatrix();
        private :
            math::Matrix2f matrix2f; /** > the base changement matrix.*/
            math::Matrix2f invMatrix2f; /**> the inverse of the base changement matrix.*/
        };
    }
}
#endif // ODFAEG_BASE_CHANGEMENT_MATRIX
