#ifndef ODFAEG_ENTITY_MANAGER_BASE_HPP
#define ODFAEG_ENTITY_MANAGER_BASE_HPP
#include <string>
#include <vector>
#include <typeinfo>
#include "../Graphics/2D/fastRenderComponent.h"
#include "export.hpp"
#include "../Graphics/baseChangementMatrix.h"
#include "../Graphics/cellMap.h"
/**
*\namespace odfaeg
* the namespace of the Opensource Development Framework Adapted for Every Games.
*/
namespace odfaeg {
/**
* \file entityManager.h
* \class EntitiesManagerHolder
* \brief base class of all entities managers of the odfaeg. (for 2D and 3D entities)
* \author Duroisin.L
* \version 1.0
* \date 1/02/2014
*/
class EntityManagerHolder {
    public :
    /**\fn
    *  \brief this function must return true if the entity manager holds 2D entities
    */
    virtual bool isG2DEntity() const = 0;

};
/**
* \file entityManager.h
* \class EntitiesManagerBase
* \brief base template class of all entities managers of the odfaeg.
* \author Duroisin.L
* \version 1.0
* \date 1/02/2014
*/
template <typename D> class EntityManagerBase: public EntityManagerHolder {
    public :
    /**
    * \fn EntityManagerBase (std::string name, bool g2dEntity)
    * \brief constructor (construct an entity manager with the given name and the given type (2D or 3D)
    * \param std::string : the name of the entity manager.
    * \param bool : the type of the entity manager. (2D or 3D)
    */
    EntityManagerBase(std::string name, bool g2dEntity) {
        this->g2dEntity = g2dEntity;
        this->name = name;
    }
    /**
    * \fn bool addEntity(E* entity)
    * \brief add en entity to the entity manager, the entity can be of any type.
    * \param E* entity : the entity to add.
    */
    template <typename E> bool addEntity(E* entity) {
        return static_cast<D*>(this)->addEntity(entity);
    }
    /**
    * \fn void generate_map(std::vector<E*> tGround, std::vector<E*> tWall, BoundingBox b)
    * \brief generate a 2D map randomly, with the given area, tiles and walls. (used for the demo and the tests)
    * \param std::vector<E*> tGround : the en entities of the ground to generate.
    * \param std::vector<E*> tWall : the entities of the walls to generate.
    */
    template <typename E> void generate_map(std::vector<E*> tGround, std::vector<E*> tWall, BoundingBox b) {
        static_cast<D*>(this)->generate_map(tGround, tWall, b);
    }
    /**
    * \fn void checkVisibleEntities ()
    * \brief check the visible entities.
    */
    void checkVisibleEntities () {
        static_cast<D*>(this)->checkVisibleEntities();
    }
    /**
    * \fn bool containsAnimatedVisibleEntity (E* entity)
    * \brief check if the entity manager contains the visible animated entity.
    *  \param the visible animated entity
    *  \return bool true if the entity manager contains the visible animated entity, false otherwise.
    */
    template <typename E> bool containsAnimatedVisibleEntity(E* entity) {
        return static_cast<D*>(this)->containsAnimatedVisibleEntity(entity);
    }
    /** \fn std::vector<E*> getVisibleEntities (std::string expression)
    * \brief return all the visible entities which are in the entity manager.
    *  \param std::string : a string which indicates which entities types to return.
    *  \return std::vector<E*> the visible entities of the entity manager.
    */
    template <typename E> std::vector<E*> getVisibleEntities (std::string expression) {
        return static_cast<D*>(this)->getVisibleEntities(expression);
    }
    /**\fn void removeVisibleEntity (E* entity)
    *  \brief remove a visible entity in the entity manager
    *  \param E : the visible entity to remove.
    */
    template <typename E> void removeVisibleEntity(E* entity) {
        static_cast<D*>(this)->removeVisibleEntity(entity);
    }
    /**\fn void insertVisibleEntity(E* entity)
    *  \brief insert a visible entity into the entity manager.
    *  \param E* : the visible entity to insert.
    */
    template <typename E> void insertVisibleEntity(E* entity) {
        static_cast<D*>(this)->insertVisibleEntity(entity);
    }
    /**\fn std::vector<E*> getEntities(std:string expression)
    *  \brief return the entities which are in the entity manager.
    *  \param std::string : a string which indicates which entities types to return.
    *  \return the entities which are in the entity manager.
    */
    template <typename E> std::vector<E*> getEntities(std::string expression) {
        return static_cast<D*>(this)->getEntities(expression);
    }
    /**\fn bool isG2DEntity() const
    *  \brief give the entity manager type.
    *  \return true if it's a 2D entity manager, false otherwise.
    */
    bool isG2DEntity() const {
        return g2dEntity;
    }
    /**\fn void drawOnComponents (std::string expression, int layer)
    *  \brief draw visible entities types on a render component.
    *  \param std::string : a string which indicates which entities types to draw.
    *  \param int : the component layer to draw entities on.
    */
    void drawOnComponents(std::string expression, int layer, sf::BlendMode mode) {
        static_cast<D*>(this)->drawOnComponents(expression, layer, mode);
    }
    void drawOnComponents(Drawable &drawable, int layer, RenderStates states) {
        static_cast<D*>(this)->drawOnComponents(drawable, layer, states);
    }
    /**\fn BasechangementMatrix getBaseChangementMatrix
    *  \brief get the base changement matrix of the entity manager.
    *  \return the base changement matrix.
    */
    BaseChangementMatrix getBaseChangementMatrix() {
        return static_cast<D*>(this)->getBaseChangementMatrix();
    }
    /**\fn std::string getName()
    *  \return the name of the entity manager.
    */
    std::string getName() {
        return name;
    }
    /**\fn E& getShadowTile()
    *  \return the shadow tile of the entity manager.
    */
    template <typename E> E& getShadowTile() {
        return static_cast<D*>(this)->getShadowTile();
    }
    /**\fn E& getLightTile()
    *  \return the light tile of the entity manager.
    */
    template <typename E> E& getLightTile(std::string expression, int n, va_list args) {
        return static_cast<D*>(this)->getLightTile(expression, n, args);
    }
    template <typename E> E& getNormalMapTile() {
        return static_cast<D*>(this)->getNormalMapTile();
    }
    /**\fn std::vector<Vec3f> getPath(Vec3f startPos, Vec3f finalPos)
    *  \brief give the path between two entities positions in the entity manager.
    *  \return the path between the two entities.
    */
    template <typename E>
    std::vector<Vec3f> getPath(E* entity, Vec3f finalPos) {
        return static_cast<D*>(this)->getPath(entity, finalPos);
    }
    /** \fn CellMap<E>* getGridCellAt(Vec3f pos)
    *   \brief get the case of a partitionning space system at the following position.
    *   \param the position.
    *   \return the case at the current position.
    */
    template <typename E>
    CellMap<E>* getGridCellAt(Vec3f pos) {
        return static_cast<D*>(this)->getGridCellAt(pos);
    }
    /** \fn void moveEntity(E* entity, float x, float y, float z)
    *   \brief move an entity into the entity manager.
    */
    template <typename E> void moveEntity(E* entity, float x, float y, float z) {
        static_cast<D*>(this)->moveEntity(entity, x, y, z);
    }
    /** \fn bool collide(E* entity)
    *   \brief test if the following entity is in collision with a case of the partitionning system, or, with another entity.
    *   \param E* the entity to test with.
    *   \return true if the entity collide, false otherwise.
    */
    template <typename E> bool collide(E* entity) {
        return static_cast<D*>(this)->collide(entity);
    }
    /** \fn std::vector<CellMap<E>*> getCaseMap()
    *   \brief get every cases of the partitioning system.
    *   \return all the cases of the partitionning system.
    */
    template <typename E> std::vector<CellMap<E>*> getCasesMap() {
        return static_cast<D*>(this)->getCasesMap();
    }
private:
    std::string name; /**> the name of the entity manager*/
    bool g2dEntity; /**>the type of the entity manager*/
    std::vector<EntityManagerHolder*> managers; /**> hold every entity managers*/
    EntityManagerHolder* currentEntityManager; /**> the current entity manager used*/
};
}
#endif // ENTITY_MANAGER
