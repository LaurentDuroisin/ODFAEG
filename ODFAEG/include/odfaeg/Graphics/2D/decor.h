#ifndef ODFAEG_DECOR_2D_HPP
#define ODFAEG_DECOR_2D_HPP
#include <SFML/Graphics.hpp>
#include "../tile.h"
#include "shadowTile.h"
#include "shadowWall.h"
#include "../model.h"
#include "../../Graphics/world.h"
/**
  *\namespace odfaeg
  * the namespace of the Opensource Development Framework Adapted for Every Games.
  */
namespace odfaeg {
    namespace graphic {
        namespace g2d {
            /**
              * \file decor.h
              * \class Decor
              * \brief Represent a decor. (It can bee a tree, an house, ..., every objects except a wall.)
              * \author Duroisin.L
              * \version 1.0
              * \date 1/02/2014
              */
            class ODFAEG_GRAPHICS_API Decor : public Selectable, public Model {
            public :
                /**
                  * \fn Decor()
                  * \brief constructor.
                  */
                Decor() : Model(math::Vec3f(0, 0, 0), math::Vec3f(1, 1, 1), math::Vec3f(0.5f, 0.5f, 0.5f),"E_DECOR"){
                    height = 0;
                    shadowCenter = math::Vec3f(0.f, 0.f, 0.f);
                    shadowScale = math::Vec3f(1.f, 1.f, 1.f);
                    shadowRotationAxis = math::Vec3f::zAxis;
                    shadowRotationAngle = 0.f;
                }
                /**
                  * \fn Decor(Tile *t, Light* light, int height, Shadow::SHADOW_TYPE shadowType)
                  * \brief constructor.
                  * \param Tile tile : the tile of the decor.
                  * \param Light* light : the light, used to generate the shadow of the decor. (generally this is the ambient light)
                  * \param height : the height of the decor.
                  * \param Shadow::SHADOW_TYPE : the type of shadow to generate. (A shape, or a black sprite)
                  */
                Decor(Tile *t, Light *light, int height, Shadow::SHADOW_TYPE shadowType);
                Entity* clone();
                bool operator== (Entity &other);
                /**
                 *\fn void vtserialize(Archive & ar)
                 *\brief serialize the decor.
                 *\param the archive where to serialize the datas.
                */
                void createShadow(Light& light);
                template <typename Archive>
                void vtserialize(Archive & ar) {
                    Model::vtserialize(ar);
                    ar(shadowCenter);
                    ar(height);
                    ar(shadowType);
                    ar(shadowCenter);
                    ar(shadowScale);
                    ar(shadowRotationAxis);
                    ar(shadowRotationAngle);
                }
                /** \fn bool isAnimated
                *   \brief return false because the class is not a class of an animated entity.
                *   \return bool false. (unanimated entity)
                */
                bool isAnimated() const {
                    return false;
                }
                /** \fn bool haveShadow()
                *   \brief return false because the class is not a class of an animated entity.
                *   \return bool false. (unanimated entity)
                */
                bool haveShadow() const {
                     return true;
                }
                /** \fn bool selectable()
                *   \brief return true because the class is an entity that you can select.
                *   \return bool true. (selectable entity)
                */
                bool selectable () const {
                    return true;
                }
                /** \fn bool isLight()
                *   \brief return false because the class is a class of an entity that isn't a light.
                *   \return bool false. (unligthed entity)
                */
                bool isLight() const {
                    return false;
                }
                /** \fn bool isShadow
                *   \brief return false because the class is not a class of a shadow entity.
                *   \return bool false. (unshadowedentity)
                */
                bool isShadow() const {
                    return false;
                }
                /** \fn int getHeight()
                *   \brief return the height of the entity.
                *   \return int. (the height of the entity)
                */
                int getHeight() {
                    return height;
                }
                void recreateShadow(Light* light);
                 /**
                  *\fn setShadowCenter(math::Vec3f shadowCenter)
                  *\brief adjust the center of the generated shadow.
                  *\param math::Vec3f shadowCenter : the center of the shadow.
                */
                void setShadowCenter(math::Vec3f shadowCenter);
                /**
                  *\fn getShadowCenter()
                  *\brief get the center of the shadow.
                  *\return math::Vec3f : the center of the shadow.
                */
                math::Vec3f getShadowCenter();
                void setShadowScale(math::Vec3f shadowScale);
                void setShadowRotation(float angle, math::Vec3f axis = math::Vec3f::zAxis);
                math::Vec3f getShadowRotationAxis();
                float getShadowRotationAngle();
                math::Vec3f getShadowScale();
            private:
                int height; /**> The height of the entity*/
                math::Vec3f shadowCenter, shadowScale, shadowRotationAxis; /**> The center of the shadow of the entity.*/
                float shadowRotationAngle;
                Shadow::SHADOW_TYPE shadowType; /** The type of the shadow of the entity.*/
            };
        }
    }
}
#endif // DECOR_H
