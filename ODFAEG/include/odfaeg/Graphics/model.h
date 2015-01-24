#ifndef ODFAEG_MODEL_2D_HPP
#define ODFAEG_MODEL_2D_HPP
#include "shadow.h"
/**
  *\namespace odfaeg
  * the namespace of the Opensource Development Framework Adapted for Every Games.
  */
namespace odfaeg {
    namespace graphic {
/**
* \file model.h
* \class Model
* \author Duroisin.L
* \version 1.0
* \date 1/02/2014
* \brief represent a 2D model, with a shadow and some other information to generate the shadows,
* and to check the intersections between the shadows and the lights.
*/
class ODFAEG_GRAPHICS_API Model : public Entity {
protected :
    /**
    * \fn Model (math::Vec3f position, math::Vec3f size, math::Vec3f origin, std::string type = "E_MODEL", Entity *parent = nullptr)
    * \brief constructor
    * \param position : the position of the model.
    * \param size : the size of the model.
    * \param the type of the model. (E_MODEL by default)
    * \param the parent entity of the model. (null by default)
    */
    Model (math::Vec3f position, math::Vec3f size, math::Vec3f origin, std::string type = "E_MODEL", Entity *parent = nullptr);
public :
    virtual void createShadow(Light& light) = 0;
    /**
    * \fn virtual void setShadowCenter (math::Vec3f shadCenter) = 0
    * \brief the function to redefine to reset the center of the shadow.
    * \param shadCenter : the center of the shadow.
    */
    virtual void setShadowCenter (math::Vec3f shadCenter) = 0;
    /**
    * \fn void setShadowCenter (math::Vec3f shadCenter)
    * \brief virtual function to redefine to get the center of the shadow.
    * \return the center of the shadow.
    */
    virtual math::Vec3f getShadowCenter() = 0;
    virtual void setShadowScale(math::Vec3f scale) = 0;
    virtual math::Vec3f getShadowScale() = 0;
    virtual void setShadowRotation(float angle, math::Vec3f axis) = 0;
    virtual float getShadowRotationAngle() = 0;
    virtual math::Vec3f getShadowRotationAxis() = 0;
    /**
    * \fn int getHeight()
    * \brief virtual function to redefine to get the height of the model.
    (The height is used to generate the shadow)
    * \return the heigth of the model.
    */
    virtual int getHeight() = 0;
    /**
    * \fn bool isModel() const;
    * \brief redefine the function isModel() from the base class entity.
    * \return if the entity is a model.
    */
    bool isModel() const;
    /**
    * \fn bool isAnimated() const;
    * \brief redefine the function isAnimated() from the base class entity.
    * \return if the entity is animated.
    */
    bool isAnimated() const;
    /**
    * \fn bool isLeaf() const;
    * \brief redefine the function isLeaf() from the base class entity.
    * \return if the entity is a leaf.
    */
    bool isLeaf() const;
    /**
    * \fn void addSegment (math::Ray *segment);
    * \brief add a segment, this segment'll cut the light with the entity.
    * \param math::Ray segment : the segment.
    */
    void addSegment (math::Ray *segment);
    /**
    * \fn void removeSegment (math::Ray *segment);
    * \brief remove a segment.
    */
    void removeSegment (math::Ray *segment);
    /**
    * \fn void addShadowPoint (math::Vec3f *point);
    * \brief add a shadow point used to generate the shadow if the shadow is a convex shape.
    * \param point : the point to add.
    */
    void addShadowPoint (math::Vec3f *point);
    /**
    * \fn  void removeShadowPoint (math::Vec3f *point);
    * \brief remove a shadow point.
    * \param the shadow point to remove.
    */
    void removeShadowPoint (math::Vec3f *point);
    /**
    * \fn std::vector<math::Ray*> getSegments() const
    * \brief get the segments used to cut the light with the model.
    * \return the segments.
    */
    std::vector<math::Ray*> getSegments() const;
    /**
    * \fn std::vectormath::Vec3f*> getPoints() const
    * \brief get the points used to generate the shadow of the given model.
    * \return the points used to generate the shadow.
    */
    std::vector<math::Vec3f*> getPoints() const;
    /**
    * \fn void vtserialize(Archive & ar)
    * \brief serialize a model into the given archive.
    * \param the archive.
    */
    template <typename Archive>
    void vtserialize(Archive & ar) {
        Entity::vtserialize(ar);
        ar(segments);
        ar(points);
    }
    /**
     *\fn virtual ~Model ();
     *\brief destructor.
    */
    virtual ~Model ();
private :
    std::vector<math::Ray*> segments; /**> the segments of the model used to check the intersections with the lights.*/
    std::vector<math::Vec3f*> points; /**> the points of the model used to project the shadow of convex shape's type.*/
};
}
}
#endif // ENTITY_WITH_SHADOW

