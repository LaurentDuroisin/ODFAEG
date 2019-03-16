#include "../../../include/odfaeg/Graphics/gridMap.h"
namespace odfaeg {
    namespace graphic {
        using namespace std;
        using namespace sf;


        //Ajoute une tile.

        GridMap::GridMap (int cellWidth, int cellHeight) {
            nbCasesPerRow = 0;
            minX = minY = std::numeric_limits<int>::max();
            maxX = maxY = std::numeric_limits<int>::min();
            this->cellWidth = cellWidth;
            this->cellHeight = cellHeight;
            offsetX = cellWidth * 0.5f;
            offsetY = cellHeight * 0.5f;
        }

        int GridMap::getCellWidth() {
            return cellWidth;
        }

        int GridMap::getCellHeight() {
            return cellHeight;
        }
        int GridMap::getNbCasesPerRow () {
            return nbCasesPerRow;
        }
        void GridMap::setBaseChangementMatrix(BaseChangementMatrix bm) {
            this->bm = bm;
        }

        BaseChangementMatrix GridMap::getBaseChangementMatrix() {
            return bm;
        }

        bool GridMap::addEntity (Entity *entity) {
            int x = entity->getGlobalBounds().getPosition().x;
            int y = entity->getGlobalBounds().getPosition().y;
            int endX = (x + entity->getGlobalBounds().getWidth());
            int endY = (y + entity->getGlobalBounds().getHeight());
            bool added = false;
            /*std::array<math::Vec2f, 4> pos;
            pos[0] = math::Vec2f(x, y);
            pos[1] = math::Vec2f(x, y + endY);
            pos[2] = math::Vec2f(x + endX, y + endY);
            pos[3] = math::Vec2f(x + endX, y);

            for (unsigned int i = 0; i < pos.size(); i++) {
                if (!(containsEntity(entity, pos[i]))) {
                    CellMap *cm = getGridCellAt(pos[i]);
                    if (cm == nullptr) {
                        createCellMap(pos[i]);
                        cm = getGridCellAt(pos[i]);
                    }
                    added = true;
                    cm->addEntity(entity);
                }
            }*/
            for (int i = x; i <= endX; i+= offsetX) {
                for (int j = y; j <= endY; j+= offsetY)  {
                    math::Vec2f pos (i, j);
                    if (!(containsEntity(entity, pos))) {
                        CellMap *cm = getGridCellAt(pos);
                        if (cm == nullptr) {
                            createCellMap(pos);
                            cm = getGridCellAt(pos);
                        }
                        added = true;
                        cm->addEntity(entity);
                    }
                }
            }
            return added;
        }

        bool GridMap::containsEntity(Entity *entity, math::Vec2f pos) {
            CellMap *caseMap = getGridCellAt(pos);
            if (caseMap !=nullptr) {
                 if (caseMap->containsEntity(entity)) {
                     return true;
                 }
            }
            return false;
        }

        Entity* GridMap::getEntity (int id) {
            for (unsigned int i = 0; i < casesMap.size(); i++) {
                CellMap *cm = casesMap[i];
                if (cm != nullptr) {
                    for (unsigned int j = 0; j < cm->getEntitiesInside().size(); j++) {
                        Entity *entity = cm->getEntityInside(j);
                        if (entity->getId() == id) {
                            return entity;
                        }
                    }
                }
            }
            return nullptr;
        }

        void GridMap::createCellMap (math::Vec2f &point) {
            math::Vec2f coordsCaseP = getCoordinatesAt(point);
            int deltaX, deltaY;
            if (coordsCaseP.x + 1 < 0)
                deltaX = -cellWidth;
            else
                deltaX = cellWidth;
            if (coordsCaseP.y + 1 < 0)
                deltaY = - cellHeight;
            else
                deltaY = cellHeight;
            minX = (coordsCaseP.x < minX) ? coordsCaseP.x : minX;
            minY = (coordsCaseP.y < minY) ? coordsCaseP.y : minY;
            maxX = (coordsCaseP.x > maxX) ? coordsCaseP.x : maxX;
            maxY = (coordsCaseP.y > maxY) ? coordsCaseP.y : maxY;

            math::Vec3f p = bm.unchangeOfBase(point);
            math::Vec3f v1;
            v1.x = (int) p.x / cellWidth * cellWidth;
            v1.y = (int) p.y / cellHeight * cellHeight;

            math::Vec2f v[4];
            v[0] = math::Vec2f (v1.x, v1.y);
            v[1] = math::Vec2f (v1.x + deltaX, v1.y);
            v[2] = math::Vec2f (v1.x + deltaX, v1.y + deltaY);
            v[3] = math::Vec2f (v1.x, v1.y + deltaY);

            for (unsigned int i = 0; i < 4; i++) {
                v[i] = bm.changeOfBase(v[i]);
            }
            physic::BoundingPolyhedron *bp = new physic::BoundingPolyhedron(v[0], v[1], v[2], true);
            bp->addTriangle(v[0], v[2], v[3]);
            CellMap *cell = new CellMap(bp, coordsCaseP);

            nbCasesPerRow = math::Math::abs(minX) + maxX + 1;
            int nbCasesPerCol = math::Math::abs(minY) + maxY + 1;
            unsigned int newSize = nbCasesPerCol * nbCasesPerRow;
            int indice = (math::Math::abs(minX) + coordsCaseP.x)
                         + (math::Math::abs(minY) + coordsCaseP.y) * nbCasesPerRow;
            if (newSize > casesMap.size()) {
                vector<CellMap*> tmpCasesMap = casesMap;
                casesMap.clear();
                casesMap.resize(newSize);
                std::fill(casesMap.begin(), casesMap.end(), nullptr);
                for (unsigned int i = 0; i < tmpCasesMap.size(); i++) {
                    if (tmpCasesMap[i] != nullptr) {
                        math::Vec2f coords = tmpCasesMap[i]->getCoords();
                        int newInd = (math::Math::abs(minX) + coords.x)
                                     + (math::Math::abs(minY) + coords.y) * nbCasesPerRow;
                        casesMap[newInd] = tmpCasesMap[i];
                    }
                }
            }
            casesMap[indice] = cell;
        }

        void GridMap::replaceEntity (Entity *entity) {
            removeEntity(entity);
            addEntity(entity);
        }

        //Supprime une tile dans la cellule. (Sans la supprimer de la mémoire.)
        bool GridMap::removeEntity (Entity *entity) {

            int x = entity->getGlobalBounds().getPosition().x;
            int y = entity->getGlobalBounds().getPosition().y;
            int endX = (x + entity->getGlobalBounds().getWidth());
            int endY = (y + entity->getGlobalBounds().getHeight());
            bool removed = false;
            for (int i = x; i <= endX; i+= offsetX) {
                for (int j = y; j <= endY; j+= offsetY) {
                    math::Vec2f pos (i, j);
                    CellMap *cellMap = getGridCellAt(pos);
                    if (cellMap != nullptr) {
                      if(cellMap->removeEntity(entity) && !removed)
                        removed = true;
                      if (!cellMap->isEntityInside())
                            removeCellMap(cellMap);
                    }
                }
            }
            return removed;
        }

        bool GridMap::deleteEntity (Entity *entity) {

            int x = entity->getGlobalBounds().getPosition().x;
            int y = entity->getGlobalBounds().getPosition().y;
            int endX = (x + entity->getGlobalBounds().getWidth());
            int endY = (y + entity->getGlobalBounds().getHeight());
            bool removed = false;
            for (int i = x; i <= endX; i+= offsetX) {
                for (int j = y; j <= endY; j+= offsetY) {
                    math::Vec2f pos (i, j);
                    CellMap *cellMap = getGridCellAt(pos);
                    if (cellMap != nullptr) {
                      if(cellMap->removeEntity(entity) && !removed)
                        removed = true;
                      if (!cellMap->isEntityInside())
                            removeCellMap(cellMap);
                    }
                }
            }
            delete entity;
            return removed;
        }

        bool GridMap::deleteEntity(int id) {

            vector<Entity*>::iterator it;
            vector<Entity*> entities = getEntities();
            for (it = entities.begin(); it != entities.end();) {

                if ((*it)->getId() == id) {
                    if (!deleteEntity(*it))
                        return false;
                    it = entities.erase(it);
                    return true;
                } else
                    it++;
            }
            return false;
        }

        void GridMap::removeCellMap (CellMap *cell) {

            for (unsigned int i = 0; i < casesMap.size(); i++) {
                if (casesMap[i] != nullptr && casesMap[i]==cell) {
                    delete casesMap[i];
                    casesMap[i] = nullptr;
                }
            }
            //Supprime les cases vides à la fin du vecteur.
            //On recherche les coordonnées de la case la plus petite.
            checkExts();
            //On cherche si il faut réduire la taille du vecteur. (En partant du début.)
            nbCasesPerRow = math::Math::abs(minX) + maxX + 1;
            int nbCasesPerCol = math::Math::abs(minY) + maxY + 1;
            unsigned int newSize = nbCasesPerCol * nbCasesPerRow;

            if (newSize < casesMap.size()) {
                vector<CellMap*> tmpCasesMap = casesMap;
                casesMap.clear();
                casesMap.resize(newSize);
                std::fill(casesMap.begin(), casesMap.end(), nullptr);
                for (unsigned int i = 0; i < tmpCasesMap.size(); i++) {
                    if (tmpCasesMap[i] != nullptr) {
                        math::Vec3f coords = tmpCasesMap[i]->getCoords();
                        int newInd = math::Math::abs(minX) + coords.x + (math::Math::abs(minY) + coords.y) * nbCasesPerRow;
                        casesMap[newInd] = tmpCasesMap[i];
                    }
                }
            }
        }

        vector<CellMap*> GridMap::getCasesInBox (physic::BoundingBox bx) {

            vector<CellMap*> cells;
            int x = bx.getPosition().x;
            int y = bx.getPosition().y;
            int endX = (x + bx.getWidth());
            int endY = (y + bx.getHeight());
            for (int i = x; i <= endX; i+= offsetX) {
                for (int j = y; j <= endY; j+= offsetY) {
                    math::Vec2f p (i, j);
                    CellMap *cell = getGridCellAt(p);
                    if (cell != nullptr) {
                        bool contains = false;
                        for (unsigned int i = 0; i < cells.size(); i++) {
                            if (cells[i] == cell)
                                contains = true;
                        }
                        if (!contains)
                            cells.push_back(cell);
                    }
                }
            }
            return cells;
        }

        vector<Entity*> GridMap::getEntitiesInBox(physic::BoundingBox box) {
            vector<Entity*> entities;
            int x = box.getPosition().x;
            int y = box.getPosition().y;
            int z = box.getPosition().z;
            int endX = box.getPosition().x + box.getWidth();
            int endY = box.getPosition().y + box.getHeight();
            int endZ = box.getDepth();
            physic::BoundingBox bx (x, y, z, endX-bx.getPosition().x, endY-bx.getPosition().y, endZ);
            for (int i = x; i <= endX; i+=offsetX) {
                for (int j = y; j <= endY; j+=offsetY) {
                    math::Vec3f point(i, j, 0);
                    CellMap* cell = getGridCellAt(point);
                    if (cell != nullptr) {
                        for (unsigned int n = 0; n < cell->getEntitiesInside().size(); n++) {
                           Entity* entity = cell->getEntityInside(n);
                           physic::BoundingBox bx2 = entity->getGlobalBounds();
                           bool contains = false;
                           for (unsigned int k = 0; k < entities.size() && !contains; k++) {
                                if (entities[k] == entity)
                                    contains = true;
                           }
                           if (!contains && bx.intersects(bx2))
                            entities.push_back(entity);
                        }
                    }
                }
            }
        }

        vector<Entity*> GridMap::getEntities () {
            vector<Entity*> allEntities;
            for (unsigned int i = 0; i < casesMap.size(); i++) {
                CellMap *cell = casesMap[i];
                if (cell != nullptr) {
                     for (unsigned int n = 0; n < cell->getNbEntitiesInside(); n++) {
                        bool contains = false;
                        for (unsigned int j = 0; j < allEntities.size(); j++) {
                            if (allEntities[j] == cell->getEntityInside(n))
                                contains = true;
                        }
                        if (!contains) {
                            allEntities.push_back(cell->getEntityInside(n));
                        }
                    }
                }
            }
            return allEntities;
        }

        math::Vec2f GridMap::getMins () {
            return math::Vec2f(minX, minY);
        }

        CellMap* GridMap::getGridCellAt (math::Vec2f point) {
            math::Vec2f coordsCaseP = getCoordinatesAt(point);
            unsigned int indice = (math::Math::abs(minX) + coordsCaseP.x) + (math::Math::abs(minY) + coordsCaseP.y) * nbCasesPerRow;
            if (indice >= 0 && indice < casesMap.size()) {
                return casesMap[indice];
            }
            return nullptr;
        }

        math::Vec2f GridMap::getCoordinatesAt(math::Vec2f &point) {
            math::Vec2f p = bm.unchangeOfBase(point);
            if (cellWidth > 0)
                p.x /= cellWidth;
            else
                p.x = 0;
            if (cellHeight > 0)
                p.y /= cellHeight;
            else
                p.y = 0;

            math::Vec2f f((int) p.x, (int) p.y);

            if (f.x <= 0 && cellWidth > 0)
                f.x--;
            if (f.y <= 0 && cellHeight > 0)
                f.y--;
            return f;
        }

        std::vector<CellMap*> GridMap::getCasesMap () {
            return casesMap;
        }
        bool GridMap::collideWithEntity(Entity* entity, math::Vec3f position) {
            CellMap* cell = getGridCellAt(position);
            if (cell != nullptr) {
                if (!cell->isPassable())
                    return true;
                std::vector<CellMap*> neightbours = getNeightbours(entity,cell,true);
                for (unsigned int i = 0; i < neightbours.size(); i++) {
                    if (!neightbours[i]->isPassable()) {
                        return true;
                    }
                }
                if (entity->getCollisionVolume() != nullptr) {
                    math::Vec3f t = position - entity->getCollisionVolume()->getCenter();
                    physic::BoundingVolume* cv = entity->getCollisionVolume()->clone().release();
                    cv->move(t);
                    for (unsigned int k = 0; k < cell->getEntitiesInside().size(); k++)  {
                        if (cell->getEntitiesInside()[k]->getCollisionVolume() != nullptr && cell->getEntitiesInside()[k] != entity) {
                            physic::CollisionResultSet::Info info;
                            if (cv->intersects(*cell->getEntitiesInside()[k]->getCollisionVolume(), info)) {
                                info.entity = cell->getEntitiesInside()[k];
                                info.center = cv->getCenter();
                                physic::CollisionResultSet::pushCollisionInfo(info);
                                if (cv->getChildren().size() == 0) {
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
            return false;
        }
        bool GridMap::collideWithEntity(Entity* entity) {
            std::unique_ptr<physic::BoundingVolume> bv1;
            physic::BoundingBox bx(entity->getGlobalBounds().getPosition().x, entity->getGlobalBounds().getPosition().y,entity->getGlobalBounds().getPosition().z,entity->getGlobalBounds().getWidth(),entity->getGlobalBounds().getHeight(),entity->getGlobalBounds().getDepth());
            if (entity->getCollisionVolume() != nullptr) {
                bv1 = entity->getCollisionVolume()->clone();
            }
            std::vector<CellMap*> cells = getCasesInBox(bx);
            for (unsigned int i = 0; i < cells.size(); i++) {
                for (unsigned int j = 0; j < cells[i]->getEntitiesInside().size(); j++) {
                    Entity* entity2 = cells[i]->getEntitiesInside()[j];
                    if (entity2 != entity) {
                        physic::BoundingVolume* bv2 = entity2->getCollisionVolume();
                        physic::CollisionResultSet::Info info;
                        if (bv1 != nullptr && bv2 != nullptr) {
                            if (bv1->intersects(*bv2, info)) {
                                info.entity = entity2;
                                physic::CollisionResultSet::pushCollisionInfo(info);
                                return true;
                            }
                        }
                    }
                }
                if (!cells[i]->isPassable())
                        return true;
            }
            return false;
        }

        void GridMap::checkExts () {
            minX = minY = std::numeric_limits<int>::max();
            maxX = maxY = std::numeric_limits<int>::min();
            for (unsigned int i = 0; i < casesMap.size(); i++) {
                if (casesMap[i] != nullptr) {
                    math::Vec2f point = casesMap[i]->getCellVolume()->getCenter();
                    math::Vec2f coordsCaseP = getCoordinatesAt(point);
                    minX = (coordsCaseP.x < minX) ? coordsCaseP.x : minX;
                    minY = (coordsCaseP.y < minY) ? coordsCaseP.y : minY;
                    maxX = (coordsCaseP.x > maxX) ? coordsCaseP.x : maxX;
                    maxY = (coordsCaseP.y > maxY) ? coordsCaseP.y : maxY;
                }
            }
        }

        math::Vec2f GridMap::getSize() {
            return math::Vec2f (maxX - minX, maxY - minY);
        }
        vector<CellMap*> GridMap::getNeightbours(Entity* entity, CellMap *cell, bool getCellOnPassable) {
            math::Vec2f coords = cell->getCoords();
            vector<CellMap*> neightbours;
            for (int i = coords.x - 1; i <= coords.x + 1; i++) {
                for (int j = coords.y - 1; j <= coords.y + 1; j++) {
                    if (!(i == coords.x && j == coords.y)) {
                        math::Vec2f neightbourCoords(i, j);
                        CellMap *neightbour = getGridCellAtFromCoords(neightbourCoords);
                        if (neightbour != nullptr) {
                            if (getCellOnPassable)
                                neightbours.push_back(neightbour);
                            else {
                                if (entity->getCollisionVolume() != nullptr) {
                                    bool collide = false;
                                    math::Vec3f t = neightbour->getCenter() - entity->getCollisionVolume()->getCenter();
                                    std::unique_ptr<physic::BoundingVolume> cv = entity->getCollisionVolume()->clone();
                                    cv->move(t);
                                    for (unsigned int k = 0; k < neightbour->getEntitiesInside().size() && !collide; k++) {
                                        if (neightbour->getEntitiesInside()[k]->getCollisionVolume() != nullptr && neightbour->getEntitiesInside()[k] != entity) {
                                            physic::CollisionResultSet::Info info;
                                            if (cv->intersects(*neightbour->getEntitiesInside()[k]->getCollisionVolume(), info)) {
                                                if (cv->getChildren().size() == 0) {
                                                    collide = true;
                                                }
                                            }
                                        }
                                    }
                                    if (!collide) {
                                        neightbours.push_back(neightbour);
                                    }
                                } else {
                                    if (neightbour->isPassable()) {
                                        neightbours.push_back(neightbour);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            return neightbours;
        }

        vector<math::Vec2f> GridMap::getPath (Entity* entity, math::Vec2f finalPos) {
            math::Vec2f startPos = math::Vec2f(entity->getCenter().x, entity->getCenter().y);
            vector<math::Vec2f> positions;
            if (getGridCellAt(finalPos) != nullptr) {
                unsigned int cpt = 0;
                positions.push_back(startPos);
                math::Vec2f currentPos = getGridCellAt(startPos)->getCenter();
                if (currentPos != startPos)
                    positions.push_back(currentPos);
                vector<CellMap*> children;
                while (!getGridCellAt(finalPos)->isPassable() && cpt < 1000) {
                    CellMap *parent = getGridCellAt(finalPos);

                    vector<CellMap*> children = getNeightbours(entity, parent, true);

                    int distMin = children[0]->getCenter().computeDist(getGridCellAt(startPos)->getCenter());
                    int indMin = 0;
                    for (unsigned int i = 1; i < children.size(); i++) {
                        std::cout<<children[i]->getCenter()<<std::endl;
                        int dist = children[i]->getCenter().computeDist(getGridCellAt(startPos)->getCenter());
                        if (dist < distMin) {
                            distMin = dist;
                            indMin = i;
                        }
                    }
                    finalPos = children[indMin]->getCenter();
                    cpt++;
                }
                //Tant qu'on est pas arrivé sur la case finale.
                while (getGridCellAt(currentPos) != getGridCellAt(finalPos) && cpt < 1000) {
                    //On recherche les cases fils. (ou voisines.)

                    CellMap *parent = getGridCellAt(currentPos);
                    parent->setTraveled(true);
                    vector<CellMap*> children = getNeightbours(entity, parent, false);
                    //std::cout<<"size : "<<children.size()<<std::endl;
                    unsigned int j = 0;
                    while (j < children.size() && children[j]->isTraveled())
                        j++;
                    if (j == children.size())
                        j--;
                    int distMin = children[j]->getCenter().computeDist(getGridCellAt(finalPos)->getCenter());
                    int indMin = j;
                    for (unsigned int i = j; i < children.size(); i++) {
                        if (!children[i]->isTraveled()) {
                            int dist = children[i]->getCenter().computeDist(getGridCellAt(finalPos)->getCenter());

                            if (dist < distMin) {

                                distMin = dist;
                                indMin = i;
                            }
                        }
                    }
                    currentPos = children[indMin]->getCenter();
                    if (positions.size() > 1) {
                        math::Vec2f v1 = positions[positions.size()-1] - positions[positions.size() - 2];
                        math::Vec2f v2 = currentPos - positions[positions.size()-1];
                        if (math::Math::abs(math::Math::toDegrees(v1.getAngleBetween(v2))) != 180)
                            positions.push_back(currentPos);
                    } else {
                         positions.push_back(currentPos);
                    }
                    cpt++;
                }
                if (finalPos != positions.back())
                    positions.push_back(finalPos);
                for (unsigned int i = 0; i < positions.size(); i++) {
                     getGridCellAt(positions[i])->setTraveled(false);
                }
                if (cpt == 1000)
                    positions.clear();
            }
            return positions;
        }

        CellMap* GridMap::getGridCellAtFromCoords(math::Vec2f coords) {
            int indice = (math::Math::abs(minX) + coords.x) + (math::Math::abs(minY) + coords.y) * nbCasesPerRow;
            if (indice >= 0 && indice < static_cast<int>(casesMap.size()))
                return casesMap[indice];
            return nullptr;
        }

        GridMap::~GridMap () {
            vector<Entity*> entities = getEntities();
            for (unsigned int i = 0; i < entities.size(); i++) {
                delete entities[i];
            }
            for (unsigned int i = 0; i < casesMap.size(); i++) {
                 if (casesMap[i] != nullptr)
                    delete casesMap[i];
            }
            casesMap.clear();
        }
    }
}



