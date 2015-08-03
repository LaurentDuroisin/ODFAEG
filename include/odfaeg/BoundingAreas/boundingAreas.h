#ifndef ODFAEG_BOUNDING_VOLUME_HPP
#define ODFAEG_BOUNDING_VOLUME_HPP
/**
  *\namespace odfaeg
  * the namespace of the Opensource Development Framework Adapted for Every Games.
  */
namespace odfaeg {
/**
  * \file boundingVolume.h
  * \class BoudingVolume
  * \brief Manage a bounding volume for collision detection
  * \author Duroisin.L
  * \version 1.0
  * \date 1/02/2014
  *
  * Base class of all bouding volumes of the framework used for collision detection.
  *
  */
class BoundingVolume {
public :
    /** \fn bool intersects (BoundingAreas &other)
    * \brief this method can be redefined in the sub class to test if two bounding volumes
    * are in collision. (if the method isn't redefined it always return false.
    * We cannot made this methode abstract because, we need to overload the intersects method in subclasses
    * for each volumes types.
    * \param the other bounding volume to test with.
    * \return return true if the two bounding volumes are in collision.
    */
    virtual bool intersects (BoundingVolume &other) {
        return false;
    }
protected :
    /** \fn BoundingVolume ()
    *   \brief constructor : we cannot create bounding volumes directly, we need to use one of its sub classes.
    */
    BoundingVolume() {}
};
}
#endif // BOUNDING_AREAS
