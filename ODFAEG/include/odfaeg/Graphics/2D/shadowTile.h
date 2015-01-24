#ifndef ODFAEG_SHADOW_TILE_2D_HPP
#define ODFAEG_SHADOW_TILE_2D_HPP
#include "shadow.h"
#include "../../../../include/odfaeg/Math/matrix2.h"
#include "../sprite.h"
#include "../model.h"
#include "ambientLight.h"
/**
  *\namespace odfaeg
  * the namespace of the Opensource Development Framework Adapted for Every Games.
  */
namespace odfaeg {
    namespace graphic {
    namespace g2d {
            class Decor;
            /**
            * \file shadowTile.h
            * \class ShadowTile
            * \author Duroisin.L
            * \version 1.0
            * \date 1/02/2014
            * \brief represent a sprite of a shadow
            */
            class ODFAEG_GRAPHICS_API ShadowTile : public Shadow {
                public :
                    /**
                    *\fn ShadowTile()
                    * \brief constructor.
                    */
                    ShadowTile() {
                        shadow = nullptr;
                    }
                    /**
                    * \fn ShadowTile (const Light& light, Model* decor);
                    * \brief constructor.
                    * \param light : the light.
                    * \param decod : the decor.
                    */
                    ShadowTile (const Light& light, Model* decor);
                    /**
                    * \fn Drawable* getShadow ();
                    * \brief get the shadow of the tile.
                    * \return the shadow.
                    */
                    Drawable* getShadow ();
                    /**
                    * \fn void createShadow (const Light &light, Model *decor);
                    * \brief get generate the shadow with the given light and the given decor.
                    * \param light : the light.
                    * \param decor : the decor.
                    */
                    void createShadow (const Light &light, Model *decor);
                    /**
                    * \fn void onDraw(RenderTarget &target, RenderStates states) const
                    * \brief draw the shadow on the given target.
                    * \param target : the render target.
                    * \param states : the render states.
                    */
                    void onDraw(RenderTarget &target, RenderStates states) const;
                    /**
                    * \fn void onMove(math::Vec3f &t);
                    * \brief redefinition of the methode of the entity base class.
                    * \param math::Vec3f the translation vector.
                    */
                    void onMove(math::Vec3f &t);
                    /**
                    * \fn void changeCenter(math::Vec3f center);
                    * \brief change the center of the shadow
                    * \param center : the center.
                    */
                    void changeCenter(math::Vec3f center);
                    /**
                    * \fn virtual bool operator== (Entity& other);
                    * \brief compare an entity with another one.
                    * \param other : the other entity.
                    */
                    virtual bool operator== (Entity& other);
                    /**
                    *\fn void vtserialize(Archive & ar)
                    *\brief serialize the entity into the given archive.
                    * \param ar the archive.
                    */
                    template <typename Archive>
                    void vtserialize(Archive & ar) {
                        Shadow::vtserialize(ar);
                    }
                    virtual ~ShadowTile();
                private :
                    Sprite *shadow; /**> sprite the shadow sprite.*/
                    math::Vec3f center; /**> the center of the shadow.*/
            };
        }
    }
}
#endif
