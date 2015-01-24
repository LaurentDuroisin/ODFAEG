#include "../../../include/odfaeg/Graphics/cellMap.h"
namespace odfaeg {
    namespace graphic {
        using namespace std;

        CellMap::CellMap (physic::BoundingPolyhedron* bp, math::Vec2f coords) {
            cellVolume = bp;
            passable = true;
            stateChanged = false;
            traveled = false;
            this->coords = coords;
        }

        math::Vec2f CellMap::getCoords () {
            return coords;
        }

        void CellMap::addEntity (Entity *entity) {

            entityInside.push_back(entity);
        }

        physic::BoundingPolyhedron* CellMap::getCellVolume () {
            return cellVolume;
        }

        bool CellMap::isEntityInside () {
            if (entityInside.size() != 0)
                    return true;
            return false;
        }

        vector<Entity*> CellMap::getEntitiesInside () {
            return entityInside;
        }

        bool CellMap::removeEntity (Entity *entity) {
            typename vector<Entity*>::iterator it;
            for (it = entityInside.begin(); it != entityInside.end();) {
                if (entity == *it) {
                    it = entityInside.erase(it);
                    return true;
                } else
                    it++;

            }
            return false;
        }

        math::Vec2f CellMap::getCenter () {
            return cellVolume->getCenter();
        }

        bool CellMap::isTraveled () {
            return traveled;
        }

        void CellMap::setTraveled (bool traveled) {
            this->traveled = traveled;
        }

        Entity* CellMap::getEntityInside (unsigned int index) {
            if (index >= 0 && index < entityInside.size()) {
                return entityInside[index];
            }
            return nullptr;
        }

        bool CellMap::containsEntity (Entity *entity) {
            for (unsigned int i = 0; i < entityInside.size(); i++)
                if (*entityInside[i] == *entity)
                    return true;
            return false;
        }

        bool CellMap::isPassable () {
            return passable;
        }

        void CellMap::setPassable (bool passable) {
            this->passable = passable;
        }

        void CellMap::setStateChanged (bool b) {
            this->stateChanged = b;
        }

        bool CellMap::isStateChanged () {
            return stateChanged;
        }

        bool CellMap::operator== (const CellMap &cellMap) {
            return *cellVolume== *cellVolume;
        }

        CellMap::~CellMap () {
            entityInside.clear();

        }
    }
}


