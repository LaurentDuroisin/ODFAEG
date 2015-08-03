#ifndef ODFAEG_SHADOW_WALL_2D_HPP
#define ODFAEG_SHADOW_WALL_2D_HPP
#include "shadow.h"
#include "../convexShape.h"
#include "ambientLight.h"
#include "../model.h"
/**
  *\namespace odfaeg
  * the namespace of the Opensource Development Framework Adapted for Every Games.
  */
namespace odfaeg {
    namespace graphic {
        namespace g2d {
            /**
            * \file shadowWall.h
            * \class ShadowWall
            * \author Duroisin.L
            * \version 1.0
            * \date 1/02/2014
            * \brief represent a convexshape of a shadow
            */
            class Wall;
            class ODFAEG_GRAPHICS_API ShadowWall : public Shadow {
                public :
                    /**
                    *\fn ShadowWall()
                    * \brief constructor.
                    */
                    ShadowWall() {
                        shadow = nullptr;
                    }
                     /**
                    *\fn ShadowWall (const Light &light, Model *model);
                    * \brief constructor.
                    * \param light : the light.
                    * \param model : the model.
                    */
                    ShadowWall (const Light &light, Model *model);
                     /**
                    *\fn createShadow (const Light &light, Model *model);
                    * \brief constructor.
                    * \param light : the light.
                    * \param model : the model.
                    */
                    void createShadow (const Light &light, Model* model);
                    /**
                    * \fn Drawable* getShadow ();
                    * \brief get the shadow of the drawable object.
                    * \return a drawable object to draw the shadow.
                    */
                    Drawable* getShadow ();
                    /**
                    * \fn void onDraw (RenderTarget &target, RenderStates states) const;
                    * \brief draw the shadow on the render target.
                    * \param target : the render target.
                    * \param states : the render states.
                    */
                    void onDraw (RenderTarget &target, RenderStates states);
                    /**
                    * \fn void onMove(math::Vec3f &t);
                    * \brief move the shadow.
                    * \param t : the translation vector.
                    */
                    void onMove(math::Vec3f &t);
                    /**
                    * \fn virtual bool operator== (Entity& other);
                    * \brief compare two entities together.
                    * \param other : the other entity.
                    */
                    virtual bool operator== (Entity& other);
                     /**
                    * \fn void changeCenter(math::Vec3f center);
                    * \brief change the center of the shadow
                    * \param center : the center.
                    */
                    void changeCenter(math::Vec3f center);
                    /**
                    *\fn void vtserialize(Archive & ar)
                    *\brief serialize the entity into the given archive.
                    *\param ar the archive.
                    */
                    template <typename Archive>
                    void vtserialize(Archive & ar) {
                        Shadow::vtserialize(ar);
                    }
                    /**
                    * \fn ~ShadowWall ();
                    * \brief destructor.
                    */
                    virtual ~ShadowWall ();
                private :
                    ConvexShape *shadow; /**the shadow to draw.*/
                    math::Vec3f center; /**the center of the shadow.*/
            };
        }
    }
}
#endif
