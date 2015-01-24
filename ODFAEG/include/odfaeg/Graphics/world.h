#ifndef WORLD_H
#define WORLD_H

#include <SFML/Graphics.hpp>
#include "../Graphics/entityManager.h"

#include "../Core/entitySystem.h"
#include "../Core/timer.h"
#include "../Graphics/fastRenderComponent.h"
#include "../Graphics/baseChangementMatrix.h"
#include "../Graphics/entityManager.h"
#include <cstdarg>
/**
 *\namespace odfaeg
 * the namespace of the Opensource Development Framework Adapted for Every Games.
 */
namespace odfaeg {
    namespace graphic {
        /**
        * \file world.h
        * \class World
        * \brief This class holds every entity managers and entity systems.
        * several entity managers can be hold, but only one can be selected.
        * \author Duroisin.L
        * \version 1.0
        * \date 1/02/2014
        */
        class ODFAEG_CORE_API World {
            public :

                /**
                *    \fn std::vector<CellMap<E>*> getCasesMap()
                *    \brief get all the cells (containing the entities) of the entity manager.
                *    \return all the cells of the entity manager.
                */
                static std::vector<graphic::CellMap*> getCasesMap() {
                    if (currentEntityManager != nullptr)
                        return currentEntityManager->getCasesMap();
                }
                /**
                *    \fn bool addEntity (E* entity)
                *    \brief add an entity into the engine.
                *    \param E* entity : tne entity to add.
                */
                static bool addEntity (Entity* entity) {
                    if (currentEntityManager != nullptr)
                        return currentEntityManager->addEntity(entity);
                }
                /**
                *   \fn E& getShadowMap()
                *   \return the shadow map.
                */
                static graphic::Entity& getShadowMap(std::string expression, int n...) {
                    if (currentEntityManager != nullptr) {
                        va_list args;
                        va_list copy;
                        va_copy(copy, args);
                        va_start(args, n);
                        currentEntityManager->generateStencilBuffer(expression, n, args);
                        va_start(copy, n);
                        return currentEntityManager->getShadowTile(expression, n, copy);
                    }
                }
                /**
                *  \fn E& getLightMap(std::string expression, int n...)
                *  \param std::string expression : the types of lights to draw on the light map.
                *  \param int n... : the layers of the component which have entities which can intersect with the light.
                */
                static graphic::Entity& getLightMap(std::string expression, int n...) {
                    if (currentEntityManager != nullptr) {
                        va_list args;
                        va_start(args, n);
                        return currentEntityManager->getLightTile(expression, n, args);
                    }
                }
                /** \fn getVisibleEntities (std::string expression)
                *   \brief get the visible entities of the given types.
                *   \param the types of the entities to get.
                *   \return the visible entities of the given types.
                */
                static std::vector<graphic::Entity*> getVisibleEntities (std::string expression) {
                    if (currentEntityManager != nullptr)
                        return currentEntityManager->getVisibleEntities(expression);
                }
                /** \fn bool containsAnimatedVisibleEntity(E *ae)
                *   \brief check if the animation is contained in the visible animated entities list of the entity manager.
                *   \param the animation to check with.
                */
                static bool containsVisibleEntity(Entity *ae) {
                    if (currentEntityManager != nullptr)
                        return currentEntityManager->containsVisibleParentEntity(ae);
                }
                /**
                *   \fn void moveEntity(E* entity, float x, float y, float z)
                *   \brief move an entity of the entity manager.
                *   \param E* entity : the entity to move.
                *   \param float x : the translation on x.
                *   \param float y : the translation on y.
                *   \param float z : the translation on z.
                */
                static void moveEntity(Entity* entity, float x, float y, float z){
                    if (currentEntityManager != nullptr) {
                       currentEntityManager->moveEntity(entity, x, y, z);
                    }
                }
                /** \fn bool collide(E* entity)
                *   \brief check if the entity collides with another entity of the entity manager.
                *   \param E* entity : the entity to check with.
                */
                static bool collide(Entity* entity) {
                    if (currentEntityManager != nullptr) {
                        return currentEntityManager->collide(entity);
                    }
                }
                static bool collide(Entity* entity, math::Vec3f position) {
                    if (currentEntityManager != nullptr) {
                        return currentEntityManager->collide(entity, position);
                    }
                }
                /**\fn void generateMap(std::vector<E*> tGrounds, std::vector<E*> tWalls)
                *  \brief generate a random map with the given ground's tiles and wall's tiles. (used for the tests)
                *  the walls are set in the bounds of the map. (It doesn't generate a dunjon.)
                *  \param std::vector<E*> tGrounds : the tiles of the ground to generate.
                *  \param std::vector<E*> tWalls : the tiles of the wall to generate.
                *  \param BoundingBox zone : the zone of the map to generate.
                */
                static void generate_map(std::vector<Tile*> tGround, std::vector<Tile*> tWall, math::Vec2f tileSize, physic::BoundingBox zone, bool terrain3D) {

                    if (currentEntityManager != nullptr) {
                        currentEntityManager->generate_map(tGround, tWall, tileSize, zone, terrain3D);
                    }
                }
                /** \fn void drawOnComponents(std::string expression, int layer, sf::BlendMode mode = sf::BlendMode::BlendAlpha)
                *   \brief draw the entities on a render components.
                *   \param std::string expression : the types of the entities to draw.
                *   \param int layer : the layer of the component.
                *   \param sf::BlendMode mode : the blend mode.
                */
                static void drawOnComponents(std::string expression, int layer, sf::BlendMode mode = sf::BlendAlpha) {
                    if (currentEntityManager != nullptr) {
                        currentEntityManager->drawOnComponents(expression, layer, mode);
                    }
                }
                 /** \fn void drawOnComponents(Drawable &drawable, int layer, sf::BlendMode mode = sf::BlendMode::BlendAlpha)
                *   \brief draw a drawable entity onto the component.
                *   \param Drawable& drawable : the drawable entity to draw.
                *   \param int layer : the layer of the component.
                *   \param RenderStates states : the render states.
                */
                static void drawOnComponents(Drawable &drawable, int layer, RenderStates states = graphic::RenderStates::Default) {
                    if (currentEntityManager != nullptr) {
                        currentEntityManager->drawOnComponents(drawable, layer, states);
                    }
                }
                /** \fn std::vector<E*> getEntities(std::string expression)
                *   \brief get the entities types in the entity manager.
                *   \param the types of the entities to get.
                *   \return the entities to get.
                */
                static std::vector<Entity*> getEntities(std::string expression) {
                    if (currentEntityManager != nullptr) {
                        return currentEntityManager->getEntities(expression);
                    }
                }
                /** \fn std::vector<math::Vec3f> getPath(E* entity, math::Vec3f finalPos)
                *   \brief get the path between an entity and a position.
                *   \param E* entity : the Entity.
                *   \param math::Vec3f finalPos : the position.
                *   \return the path.
                */
                static std::vector<math::Vec2f> getPath(Entity* entity, math::Vec2f finalPos) {
                    std::vector<math::Vec2f> positions;
                    if (currentEntityManager != nullptr) {
                        if (getGridCellAt(finalPos) != nullptr) {

                            unsigned int cpt = 0;
                            math::Vec2f currentPos = entity->getCenter();
                            positions.push_back(currentPos);
                            std::vector<math::Vec2f> visitedPositions;
                            std::vector<math::Vec2f> neightbourPositions;
                            while(collide(entity, finalPos) && cpt < 1000) {
                                for (int i = -50; i <= 50; i+= 50) {
                                    for (int j = -50; j <= 50; j+= 50) {
                                        if (i != 0 && j != 0) {
                                            math::Vec2f v(i, j);
                                            neightbourPositions.push_back(finalPos + v);
                                        }
                                    }
                                }
                                int distMin = neightbourPositions[0].computeDist(currentPos);
                                int indMin = 0;
                                for (unsigned int i = 1; i < neightbourPositions.size(); i++) {
                                    int dist = neightbourPositions[i].computeDist(currentPos);
                                    if (dist < distMin)  {
                                        distMin = dist;
                                        indMin = i;
                                    }
                                }
                                finalPos = neightbourPositions[indMin];
                                neightbourPositions.clear();
                                cpt++;

                            }
                            if (cpt == 1000) {
                                positions.clear();
                                return positions;
                            }
                            cpt = 0;
                            while(currentPos.computeDist(finalPos) > 50 && cpt < 1000) {
                                for (int i = -50; i <= 50; i+= 50) {
                                    for (int j = -50; j <= 50; j+= 50) {
                                        if (i != 0 && j != 0) {
                                            bool visited = false;
                                            math::Vec2f v(i, j);
                                            for (unsigned int n = 0; n < visitedPositions.size(); n++) {
                                                if (currentPos + v == visitedPositions[n])
                                                    visited = true;
                                            }
                                            if (!collide(entity, currentPos + v) && !visited) {
                                                neightbourPositions.push_back(currentPos + v);
                                            }
                                        }

                                    }
                                }
                                if (neightbourPositions.size() > 0) {
                                    int distMin = neightbourPositions[0].computeDist(finalPos);
                                    int indMin = 0;
                                    for (unsigned int i = 1; i < neightbourPositions.size(); i++) {
                                        int dist = neightbourPositions[i].computeDist(finalPos);
                                        if (dist < distMin)  {
                                            distMin = dist;
                                            indMin = i;
                                        }
                                    }
                                    currentPos = neightbourPositions[indMin];
                                    if (positions.size() > 1) {
                                        math::Vec2f v1 = positions[positions.size()-1] - positions[positions.size() - 2];
                                        math::Vec2f v2 = currentPos - positions[positions.size()-1];
                                        if (math::Math::abs(math::Math::toDegrees(v1.getAngleBetween(v2))) < 179)
                                            positions.push_back(currentPos);
                                    } else {
                                        positions.push_back(currentPos);
                                    }
                                    visitedPositions.push_back(currentPos);
                                    neightbourPositions.clear();
                                }
                                cpt++;
                            }
                            if (cpt == 1000) {
                                positions.clear();
                                return positions;
                            }
                            positions.push_back(finalPos);
                        }
                        //return currentEntityManager->getPath(entity, finalPos);
                    }
                    return positions;
                }
                /** \fn CellMap<E>* getGridCellAt(math::Vec3f pos)
                *   \brief get the cell at the given position.
                *   \param math::Vec3f pos : the position.
                *   \return the cell.
                */
                static graphic::CellMap* getGridCellAt(math::Vec3f pos) {
                    if (currentEntityManager != nullptr) {
                        return currentEntityManager->getGridCellAt(pos);
                    }
                }
                static void changeVisibleEntity(Entity* toRemove, graphic::Entity* toAdd) {
                    if (currentEntityManager != nullptr) {
                        currentEntityManager->changeVisibleEntity(toRemove, toAdd);
                    }
                }
                static void updateTimers() {
                    for (unsigned int i = 0; i < aus.size(); i++) {
                        if (!aus[i]->isUsingThread()) {
                            aus[i]->update();
                        }
                    }
                }
                static graphic::BaseChangementMatrix getBaseChangementMatrix() {
                    if (currentEntityManager != nullptr) {
                        return currentEntityManager->getBaseChangementMatrix();
                    }
                    return graphic::BaseChangementMatrix();
                }
                static void addEntitiesUpdater(core::EntitySystem *eu) {
                    eus.push_back(eu);
                }
                static void addAnimUpdater(core::Timer *au) {

                    aus.push_back(au);
                }
                static void update() {
                    for (unsigned int i = 0; i < eus.size(); i++) {
                        eus[i]->update();
                    }
                }
                static void computeIntersectionsWithWalls() {
                    if (currentEntityManager != nullptr)
                        currentEntityManager->computeIntersectionsWithWalls();
                }
                static void addEntityManager(graphic::EntityManager* holder) {
                    ems.push_back(holder);
                }
                static void checkVisibleEntities() {
                    if (currentEntityManager != nullptr) {

                        currentEntityManager->checkVisibleEntities();
                    }
                }
                static void removeEntityManager (std::string emName) {
                    std::vector<EntityManager*>::iterator it;
                    for (it = ems.begin(); it != ems.end();) {
                        std::string otherName = (*it)->getName();
                        if (emName == otherName) {
                            it = ems.erase(it);
                            if (currentEntityManager == *it)
                                currentEntityManager = nullptr;
                        } else {
                            it++;
                        }
                    }
                }
                static void setCurrentEntityManager (std::string mapName) {

                    std::vector<EntityManager*>::iterator it;
                    for (it = ems.begin(); it != ems.end(); it++) {
                        std::string otherName = (*it)->getName();
                        if (otherName == mapName) {
                           currentEntityManager = *it;
                        }
                    }
                }
            private :
                static std::vector<core::EntitySystem*> eus; /**> holds every entity systems*/
                static std::vector<core::Timer*> aus; /**> holds every timers.*/
                static std::vector<EntityManager*> ems; /**> holds every entity managers*/
                static graphic::EntityManager* currentEntityManager; /**> holds the current entity manager.*/
        };
    }
}
#endif
