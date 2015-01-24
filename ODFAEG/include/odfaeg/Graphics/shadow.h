#ifndef ODFAEG_SHADOW_2D_HPP
#define ODFAEG_SHADOW_2D_HPP


#include "light.h"
#include "entity.h"
/**
  *\namespace odfaeg
  * the namespace of the Opensource Development Framework Adapted for Every Games.
  */
namespace odfaeg {
    namespace graphic {
         class Model;
        /**
        * \file shadow.h
        * \class Shadow
        * \author Duroisin.L
        * \version 1.0
        * \date 1/02/2014
        * \brief represent a shadow
        */
        class ODFAEG_GRAPHICS_API Shadow : public Entity {

            public :
                /**
                * \fn Shadow()
                * \brief constructor.
                */
                Shadow();
                /**
                * \fn Shadow(const Light &light, Entity *parent, std::string type)
                * \brief constructor.
                * \param light : the light used to generate the shadow.
                * \param parent : the parent entity.
                * \param type : the type of the shadow.
                */
                Shadow(const Light &light, Entity *parent, std::string type);
                /**
                * \fn void createShadow(const Light& light, Model* model)
                * \brief generate a shadow with the given light and the given model.
                * \param light : the light.
                * \param model : the model.
                */
                virtual void createShadow(const Light& light, Model* model) = 0;
                /**
                * \fn int& getEntityId();
                * \brief get the id of the entity.
                * \return the id of the entity.
                */
                int& getEntityId();
                /**
                * \fn int& getEntityId();
                * \brief get the id of the light.
                * \return the id of the light.
                */
                int& getLightId();
                /**
                * \fn  bool isAnimated() const
                * \brief redefinition of the method of the base class entity.
                * \return false (a shadow is not an animation type)
                */
                bool isAnimated() const {
                    return false;
                }
                /**
                * \fn  bool isLeaf() const
                * \brief redefinition of the method of the base class entity.
                * \return true (a shadow haven't any children)
                */
                bool isLeaf () const {
                    return true;
                }
                /**
                * \fn  bool isModel() const
                * \brief redefinition of the method of the base class entity.
                * \return false (a shadow is not a model)
                */
                bool isModel() const {
                    return false;
                }
                 /**
                * \fn  bool selectable() const
                * \brief redefinition of the method of the base class entity.
                * \return false (a shadow is not selectable)
                */
                bool selectable() const {
                    return false;
                }
                 /**
                * \fn  bool isShadow() const
                * \brief redefinition of the method of the base class entity.
                * \return true (a shadow is a shadow)
                */
                bool isShadow() const {
                    return true;
                }
                 /**
                * \fn  bool isLight() const
                * \brief redefinition of the method of the base class entity.
                * \return false (a shadow is not a light)
                */
                bool isLight() const {
                    return false;
                }
                /**
                * \enum SHADOW_TYPE
                * \brief the type of the shadow. (a sprite or a convex shape)
                */
                enum SHADOW_TYPE {
                    SHADOW_TILE, SHADOW_WALL
                };
                /**
                * \fn void vtserialize(Archive & ar)
                * \brief serialize the entity* into the given archive.
                * \param ar : the archive.
                */
                template <typename Archive>
                void vtserialize(Archive & ar) {
                    Entity::vtserialize(ar);
                }
                /**
                * \fn Drawable* getShadow()
                * \brief method to redefine to get the drawable object which draw the shadow.
                * \return a drawable object.
                */
                virtual Drawable* getShadow() = 0;
                /**
                * \fn virtual void changeCenter(math::Vec3f center)
                * \brief change the center of the shadow of an object.
                * \param the center of the shadow.
                */
                virtual void changeCenter(math::Vec3f center) = 0;
                /**
                * \fn virtual ~Shadow ()
                * \brief destructor.
                */
                virtual ~Shadow () {}
            private :
                int *entityId, *lightId; /**> pointers to the lights and entity's id used to generate the shadow.*/

        };
    }
}
#endif
