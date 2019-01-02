#ifndef ODFAEG_MAP_2D_HPP
#define ODFAEG_MAP_2D_HPP
#include "gridMap.h"
#include "animatedEntity.h"
#include "2D/wall.h"
#include "2D/decor.h"
#include "2D/ambientLight.h"
#include "2D/ponctualLight.h"
#include "renderComponentManager.h"
#include "../Core/utilities.h"
#include "../Physics/boundingSphere.h"
#include "renderWindow.h"
#include "renderTexture.h"
#include "entityManager.h"
/**
  *\namespace odfaeg
  * the namespace of the Opensource Development Framework Adapted for Every Games.
  */
namespace odfaeg {
    namespace graphic {
/**
* \file map.h
* \class Map
* \author Duroisin.L
* \version 1.0
* \date 1/02/2014
* \brief a map used to generate and to get and to draw the entities of the scene.
*/
class ODFAEG_GRAPHICS_API Map : public EntityManager {

    public :
        /** \fn Map(RenderComponentManager* frcm, std::string name, int cellWidth, int cellHeight, int cellDepth);
        *   \brief constructor
        *   \param frcm : the component manager used to manage components.
        *   \param name : the name of the map.
        *   \param cellWidth : the width of the cells.
        *   \param cellHeight : the height of the cells.
        *   \param cellDepth : the depth of the cells.
        */
        Map(RenderComponentManager* frcm, std::string name, int cellWidth, int cellHeight);
        GridMap *gridMap; /**> The grid used to store the entities.*/
        /**
        * \fn int getId()
        * \brief get the id of the map.
        * \return the id.
        */
        int getId();
        /**
        * \fn getVersion()
        * \brief get the version of the map.
        * \return the version.
        */
        int getVersion();
        /**
        * \fn std::string getName()
        * \brief get the name of the map.
        * \return the name.
        */
        std::string getName();
        /**
        * \fn void setVersion(int version);
        * \brief set the version of the map.
        * \param version : the version.
        */
        void setVersion(int version);
        /**
        * \fn void setName(std::string name);
        * \brief set the name of the map.
        * \param std::string name : the name.
        */
        void setName (std::string name);
        /**
        * \fn void setName(std::string name);
        * \brief set the name of the map.
        * \param std::string name : the name.
        */
        void setId (int id);
        /**
        * \fn getCompImage(const void* resource)
        * \brief get the number of time that the given resource is used on the map.
        * \param resource : the resource.
        * \return how many times the resource is used.
        */
        int getCompImage(const void* resource);
        /**
        * \fn math::Vec3f getPosition()
        * \brief get the position of a map.
        * \return the position of the map.
        */
        math::Vec3f getPosition();
        /**
        * \fn int getWidth();
        * \brief get the width of the map.
        * \return the width of the map.
        */
        int getWidth();
        /**
        * \fn int getHeight();
        * \brief get the height of the map.
        * \return the height of the map.
        */
        int getHeight();
        /**
        * \fn bool addEntity(Entity *entity);
        * \brief add an entity into the map.
        * \param entity : the entity to add.
        * \return if the entity has been successfully added.
        */
        bool addEntity(Entity *entity);
        /**
        * \fn bool removeEntity(Entity *entity);
        * \brief remove an entity from the map.
        * \param entity : the entity to remove.
        * \return if the entity has been successfully removed.
        */
        bool removeEntity (Entity *entity);
        bool deleteEntity(Entity* entity);
        /**
        * \fn int getNbCasesPerRow ();
        * \brief get the number of cases per row.
        * \return the number of cells per row.
        */
        int getNbCasesPerRow ();
        /**
        * \fn bool removeEntity(int id)
        * \brief remove an entity from it's id.
        * \param the id of the entity to remove.
        * \return true if the entity has been successfully removed.
        */
        bool removeEntity(int id);
        /**
        * \fn void rotateEntity(Entity *entity, int angle);
        * \brief rotate an entity and replace it to the grid.
        * \param entity : the entity.
        * \param angle : the angle.
        */
        void rotateEntity(Entity *entity, int angle);
        /**
        * \fn void scaleEntity(Entity *entity, int angle);
        * \brief scale an entity and replace it to the grid.
        * \param entity : the entity.
        * \param sx : the x factor.
        * \param sy : the y factor.
        */
        void scaleEntity(Entity *entity, float sx, float sy);
        /**
        * \fn void moveEntity(Entity *entity, float dx, float dy, float dz);
        * \brief move an entity and replace it to the grid.
        * \param dx : the x translation.
        * \param dy : the y translation.
        * \param dz : the z translation.
        */
        void moveEntity(Entity *entity, float dx, float dy, float dz);
        /**
        * \fn  void checkVisibleEntities();
        * \brief check the entities which are visible. (Which are in the field of view of the camera)
        */
        void checkVisibleEntities();
        bool containsMovableVisibleEntity(AnimatedEntity *ae);
        /**
        * \fn  Entity* getEntity(int id);
        * \brief get an entity from its id.
        * \return entity : the entity.
        */
        Entity* getEntity(int id);
        /**
        * \fn  std::vector<CellMap<Entity>*> getCasesMap();
        * \brief get every cells of the grid.
        * \return the cells of the grid.
        */
        std::vector<CellMap*> getCasesMap();
        /**
        * \fn std::vector<Entity*> getEntities(std::string type)
        * \brief get the entities of the given types.
        * \param std::string type : the types.
        * \return the entities.
        */
        std::vector<Entity*> getEntities(std::string type);
        /**
        * \fn std::vector<Entity*> getVisibleEntities(std::string type)
        * \brief get the visible entities of the given types.
        * \param std::string type : the types.
        * \return the visible entities.
        */
        std::vector<Entity*> getVisibleEntities (std::string type);
        /**
        * \fn std::vector<Entity*> getEntitiesInBox(BoundingBox bx, std::string types)
        * \brief get the entities of the given types which are into the given box.
        * \param BoundingBox bx : the bounding box of the entity.
        * \param std::string type : the types.
        * \return the entities.
        */
        std::vector<Entity*> getEntitiesInBox (physic::BoundingBox bx, std::string types);
        /**
        * \fn math::Vec3f getCoordinatesAt(math::Vec3f p)
        * \brief get coordinates at a given point.
        * \param math::Vec3f point : the point.
        * \return the coordinates.
        */
        math::Vec2f getCoordinatesAt(math::Vec2f p);
        /**
        * \fn CellMap<Entity>* getGridCellAt(math::Vec3f p)
        * \brief get the cell at a given point.
        * \param math::Vec3f point : the point.
        * \return the cell.
        */
        CellMap* getGridCellAt(math::Vec2f p);
        /**
        * \fn std::vector<CellMap<Entity>>* getGridCellAt(math::Vec3f p)
        * \brief get the cells which are in the given box.
        * \param BoundingBox bx: the box.
        * \return the cells.
        */
        std::vector<CellMap*> getCasesInBox (physic::BoundingBox bx);
        /**
        * \fn bool containsVisibleEntity(Entity* entity);
        * \brief check if the map contains the following visible entity.
        * \param the entity.
        * \return if the map contains the entity.
        */
        bool containsVisibleEntity(Entity* entity);
        /**
        * \fn bool collide (Entity *entity)
        * \brief check if an entity collide with another one.
        * \param Entity* entity : the entity.
        * \return if an entity collide with another.
        */
        bool collide(Entity *entity);
        bool collide(Entity* entity, math::Vec3f position);
        bool collide(Entity* entity, math::Ray ray);
        /** \fn generate_map(std::vector<Tile*> tGround, std::vector<Tile*> walls, BoundingBox &box)
        *   \brief generate a map in the given zone, which the given tiles for the ground and the given tiles for the walls.
        *   \param std::vector<Tile*> tGround : the tiles used for the ground.
        *   \param std::vector<Tile*> walls : the tiles used for the walls. (The walls are placed to the edges of the map)
        */
        void generate_map(std::vector<Tile*> tGround, std::vector<Tile*> walls, math::Vec2f tileSize, physic::BoundingBox &box, bool terrain3D);
        /**
        *   \fn void getChildren (Entity *entity, std::vector<Entity*> &entities, std::string type);
        *   \brief get the children of a kind of types from an entity.
        *   \param entity : the entity.
        *   \param entities : the children entities.
        *   \param type : the types.
        */
        void getChildren (Entity *entity, std::vector<Entity*> &entities, std::string type);
        /**
        * \fn void insertVisibleEntity(Entity *entity);
        * \brief insert a visible entity into the entity manager.
        * \param entity : the visible entity to insert.
        */
        void insertVisibleEntity(Entity *entity, physic::BoundingBox bx);
        /**
        * \fn void removeVisibleEntity(Entity *entity);
        * \brief remove a visible entity from the entity manager.
        * \param entity : the visible entity to remove.
        */
        void removeAnimatedVisibleEntity(Entity *entity, std::vector<Entity*>& entities, View& view, bool& removed);
        /**
        * \fn Tile& getShadowTile();
        * \brief get the shadow tile. (The tile where all the shadows are drawn)
        * \return the shadow tile.
        */
        void generateStencilBuffer(std::string expression, int n, va_list args);
        Entity& getShadowTile(std::string expression, int n, va_list args);
        /**
        * \fn Tile& getLightTile();
        * \brief get the light tile. (The tile where all the lights are drawn)
        * \param n : the number of the layers.
        * \param the layer of the components which have entities which intersects with the light. (dereffered shading)
        * \return the light tile.
        */
        Entity& getLightTile(std::string expression, int n, va_list args);
        /**
        * \fn void drawOnComponents(std::string expression, int layer, sf::BlendMode mode = sf::BlendMode::BlendAlpha);
        * \brief draw the entities on a component.
        * \param expression : the types of entities to draw.
        * \param layer : the layer of the component on which to draw the entities.
        * \param mode: the blend mode.
        */
        Entity& getRefractionTile (std::string expression, int n, va_list args);
        void drawOnComponents(std::string expression, int layer, sf::BlendMode mode = sf::BlendAlpha);
        /**
        * \fn void drawOnComponents(Drawable& drawable, int layer, RenderStates states = RenderStates::Default);
        * \brief draw a drawable object onto a component.
        * \param layer : the layer of the component on which to draw the entities.
        * \param states : the render states.
        */
        void drawOnComponents(Drawable& drawable, int layer, RenderStates states = RenderStates::Default);
        /**
        * \fn BaseChangementMatrix getBaseChangementMatrix();
        * \brief get the base changement matrix of the map.
        * \return the base changement matrix.
        */
        BaseChangementMatrix getBaseChangementMatrix();
        /**
        * \fn std::vector<math::Vec3f> getPath(Entity* entity, math::Vec3f finalPos);
        * \brief get the path between an entity and a position.
        * \return the path between an entity and a position.
        */
        std::vector<math::Vec2f> getPath(Entity* entity, math::Vec2f finalPos);
        void setBaseChangementMatrix(BaseChangementMatrix bm);
        void updateParticles();
    private :
        /**
        * \fn void insertAnimatedVisibleEntity(Entity* ae, std::vector<Entity*>& visibleEntities);
        * \brief insert an animated visible entity into the vector of the visible entities.
        * \param ae : the animated visible entity.
        * \param std::vector<Entity*>& visibleEntities the visible entities.
        */
        void insertAnimatedVisibleEntity(Entity* ae, std::vector<Entity*>& visibleEntities, View& view);
        /**
        * \fn void increaseComptImg (const void* resource);
        * \brief increase the counter for the given resource.
        * \param resource : the resource.
        */
        void increaseComptImg (const void* resource);
        /**
        * \fn void decreaseComptImg (const void* resource);
        * \brief decrease the counter for the given resource.
        * \param resource : the resource.
        */
        void decreaseComptImg (const void* resource);
        /**
        * \fn void removeComptImg (const void* resource);
        * \brief remove the counter for the given resource.
        * \param resource : the resource.
        */
        void removeComptImg (const void* resource);
        /**
        * \fn bool containsVisibleParentEntity(Entity* entity);
        * \brief check if the entity have the same parent of an entity which is in the visible entity list.
        *  it avoids to add the children entities of a same parent more than once.
        * \param the entity.
        * \return if the entity have the same parent of an entity which is in the visible entity list.
        */
        bool containsVisibleParentEntity(Entity* entity);
        void changeVisibleEntity(Entity* toRemove, Entity* toAdd);
        std::string name; /**> the name of the map.*/
        int id, version; /**> the version of the map.*/
        Map (const Map &other); /**> A map is not copiable.*/
        Map operator= (const Map &other); /**> A map is not affectable.*/
        std::map<const void*, int> compImages; /**> Store a pointer to a resource and how many times his resource is used on the map.*/
        std::map<std::string, std::vector<Entity*>> vEntitiesByType;
        typedef std::map <std::string, std::vector<Entity*>>::iterator VEntitiesByType; /**> A typedef to the iterator of the map which contains the visible entities.*/
        std::vector<Entity*> visibleParentEntities; /**> The parent entities of the visible entities.*/
        std::vector<Entity*> lights, shadows; /**> The lights and the shadows.*/
        RenderComponentManager* frcm; /**> The component manager.*/
        std::unique_ptr<RenderTexture> shadowMap, lightMap, stencilBuffer, normalMap, backDepthBuffer, refractionMap; /**> The shadow map, the light map and the normal map.*/
        Texture textShadow, textLight; /**> The texture of the shadows and the lights.*/
        std::unique_ptr<Tile> shadowTile, lightTile, stencilBufferTile, normalMapTile, refractionTile; /**> The shadow, light and normal map tiles.*/
        std::unique_ptr<Shader> perPixLightingShader, perPixShadowShader, perPixShadowShader2, buildShadowMapShader,
        buildNormalMapShader, depthBufferGenShader, buildRefractionMapShader; /**> The shaders used to generate the lightmap and the final normal map.*/
        bool updateComponents;
        float diagSize;
};
}
}
#endif
