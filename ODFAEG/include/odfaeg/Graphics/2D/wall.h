#ifndef ODFAEG_WALL_2D_HPP
#define ODFAEG_WALL_2D_HPP

#include "../../Math/maths.h"
#include "../../Math/matrix2.h"
#include "../light.h"
#include "../model.h"
#include "../../Graphics/world.h"
#include "../selectable.h"
#include <vector>
/**
  *\namespace odfaeg
  * the namespace of the Opensource Development Framework Adapted for Every Games.
  */
namespace odfaeg {
    namespace graphic {
        namespace g2d {
            /**
              * \file wall.h
              * \class Wall
              * \brief Respresent a tile.
              * \author Duroisin.L
              * \version 1.0
              * \date 1/02/2014
              * Represent a wall.
              */
            class ODFAEG_GRAPHICS_API Wall : public Selectable, public Model {
                public :
                    /**
                    * \fn  Wall()
                    * \brief constructor.
                    */
                    Wall() : Model(math::Vec3f(0, 0, 0),math::Vec3f(1, 1, 1),math::Vec3f(0.5f, 0.5f, 0.5f), "E_WALL") {

                    }
                    /**
                    * \fn Wall (int imageId, int height, Tile *tile, Light *light, Shadow::SHADOW_TYPE shadowType);
                    * \brief constructor.
                    * \param imageId : the id of the wall.
                    * \param height : the height of the wall.
                    * \param tile : the tile of the wall.
                    * \param light : the light used to generate the shadow.
                    * \param sahdowType : the type of the shadow to generate.
                    */
                    Wall (Tile *tile, Light *light);
                    void createShadow(Light& light);
                    /**
                    * \fn bool isLeaf() const;
                    * \brief redefinition of the method of the base class Entity.
                    * \return if the entity can have children.
                    */
                    bool isLeaf() const;
                    /**
                    * \fn int getHeight();
                    * \brief return the height of the wall.
                    */
                    int getHeight();
                    /**
                    * \fn void onMove(math::Vec3f &t);
                    * \brief redefinition of the method of the base class Entity.
                    * \param t : the translation.
                    */
                    void onMove(math::Vec3f &t);
                    /**
                    * \fn int getEntityId ();
                    * \brief get the id of the entity.
                    * \return the id of the entity.
                    */
                    int getEntityId ();
                    /**
                    * \fn  void setWallType (int type);
                    * \brief set the type of the wall.
                    * \param type : the type of the wall.
                    */
                    void setWallType (int type);
                    /**
                    * \fn int getWallType ();
                    * \brief get the type of the wall.
                    * \return the type of the wall.
                    */
                    int getWallType ();
                    /**
                    * \fn bool operator== (Entity &other);
                    * \brief compare if two entities are equal.
                    * \param other : compare if an entity is equal to another.
                    */
                    bool operator== (Entity &other);
                    /**
                    * \fn  void setShadowCenter (math::Vec3f shadCenter);
                    * \brief set the center of the shadow.
                    * \param shadCenter : the center of the shadow.
                    */
                    void setShadowCenter (math::Vec3f shadCenter);
                    /**
                    * \fn math::Vec3f getShadowCenter();
                    * \brief get the center of the shadow.
                    * \return the center of the shadow.
                    */
                    math::Vec3f getShadowCenter();
                    /**
                    * \fn bool selectable() const;
                    * \brief redefinition of the method of the base class Entity.
                    * \return if the entity is selectable.
                    */
                    bool selectable () const;
                    /**
                    * \fn bool isLight() const;
                    * \brief redefinition of the method of the base class Entity.
                    * \return if the entity is a light.
                    */
                    bool isLight() const;
                    /**
                    * \fn bool isShadow() const;
                    * \brief redefinition of the method of the base class Entity.
                    * \return if the entity is a shadow.
                    */
                    bool isShadow() const;
                    /**
                    * \fn void vtserialize(Archive & ar)
                    * \brief serialize an entity into the archive.
                    * \param ar : the archive in which to serialize the entity.
                    */
                    template <typename Archive>
                    void vtserialize(Archive & ar) {
                        Model::vtserialize(ar);
                    }
            };
        }
    }
}
#endif

