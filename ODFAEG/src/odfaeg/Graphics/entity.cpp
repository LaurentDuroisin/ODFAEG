#include "../../../include/odfaeg/Graphics/entity.h"
namespace odfaeg {
    namespace graphic {
        int Entity::nbEntities = 0;
        int Entity::nbEntitiesTypes = 0;
        std::map<int, std::string>* Entity::types = initTypes();
        Entity::Entity (math::Vec3f position, math::Vec3f size, math::Vec3f origin, std::string sType, Entity *parent) :
            Transformable (position, size, origin), Drawable(), entityState("Entity State", nullptr) {
            this->parent = parent;
            types = initTypes();
            int iType = getIntOfType(sType);
            if (iType == -1) {
                type = std::pair<int, std::string> (nbEntitiesTypes, sType);
                types->insert(type);
                nbEntitiesTypes++;
            } else {
                std::map<int, std::string>::iterator it = types->find(iType);
                type = *it;
            }
            id = nbEntities;
            nbEntities++;
            alreadySerialized = false;
            collisionVolume = nullptr;
        }
        std::string Entity::getRootType() {
            if (getParent() != nullptr) {
                return parent->getRootType();
            }
            return getType();
        }
        int Entity::getNbEntities () {
            return nbEntities;
        }
        void Entity::draw (RenderTarget &target, RenderStates states) {
            states.transform = getTransform();
            onDraw(target, states);
            for (unsigned int i = 0; i < children.size(); i++) {
                children[i]->draw(target, states);
            }
        }
        void Entity::setType(std::string sType) {
            int iType = getIntOfType(sType);
            if (iType == -1) {
                type = std::pair<int, std::string> (nbEntitiesTypes, sType);
                types->insert(type);
                nbEntitiesTypes++;
            } else {
                std::map<int, std::string>::iterator it = types->find(iType);
                type = *it;
            }
        }
        std::string Entity::getType () const {
            return type.second;
        }
        void Entity::setId (int id) {
            this->id = id;
            onIDUpdated(id);
        }
        int& Entity::getId () {

            return id;
        }
        int Entity::getTypeInt () {
            return type.first;
        }
        int Entity::getIntOfType(std::string sType) {

            std::map<int, std::string>::iterator it;
            for (it = types->begin(); it != types->end(); ++it) {
                if (it->second == sType)
                    return it->first;
            }
            return -1;
        }
        std::string Entity::getTypeOfInt (int type) {
            std::map<int, std::string>::iterator it = types->find(type);
            return it->second;
        }
        void Entity::addChild (Entity* child) {
            std::vector<math::Vec3f> vecs;
            children.push_back(child);
            for (unsigned int i = 0; i < children.size(); i++) {
                vecs.push_back(children[i]->getPosition());
                vecs.push_back(children[i]->getPosition() + children[i]->getSize());
            }
            std::array<std::array<float, 2>, 3> minsMaxs;
            minsMaxs = math::Computer::getExtends(vecs);
            math::Vec3f pos((int) minsMaxs[0][0], (int) minsMaxs[1][0], (int) minsMaxs[2][0]);
            math::Vec3f size((int) minsMaxs[0][1] - (int) minsMaxs[0][0], (int) minsMaxs[1][1] - (int) minsMaxs[1][0], (int) minsMaxs[2][1] - (int) minsMaxs[2][0]);
            setLocalBounds(physic::BoundingBox(pos.x, pos.y, pos.z, size.x, size.y, size.z));
            vecs.clear();
        }
        void Entity::removeChild (Entity *child) {
            std::vector<Entity*>::iterator it;
            for (it = children.begin(); it != children.end();) {
                if (*it == child) {
                    delete *it;
                    it = children.erase(it);
                } else
                    it++;
            }
        }
        void Entity::updateTransform() {

            /*getTransform().update();
            if (!(RenderTarget::getMajorVersion() >= 3 && RenderTarget::getMinorVersion() >= 3)) {
                for (unsigned int i = 0; i < faces.size(); i++) {
                    VertexArray& va = faces[i]->getVertexArray();
                    va.transform(getTransform());
                }
            }*/
        }
        //Return the children of the entities.
        std::vector<Entity*> Entity::getChildren() const {
            return children;
        }

        //Return the number of entity's children.
        unsigned int Entity::getNbChildren () {
            return children.size();
        }
        void Entity::setParent(Entity *parent) {
            this->parent = parent;
        }
        Entity* Entity::getParent() const {
            return parent;
        }

        void Entity::onMove(math::Vec3f &t) {
            updateTransform();
            for (unsigned int i = 0; i < children.size(); i++) {
                children[i]->move(t);
            }


        }
        void Entity::onScale(math::Vec3f &s) {
            updateTransform();
            for (unsigned int i = 0; i < children.size(); i++) {
                children[i]->scale(s);
            }

        }
        void Entity::onRotate(float angle) {
            updateTransform();
            for (unsigned int i = 0; i < children.size(); i++) {
                children[i]->rotate(angle);
            }
        }
        void Entity::addFace (Face* face) {
            faces.push_back(face);
        }
        std::vector<Face*> Entity::getFaces() const {
            return faces;
        }
        Entity::~Entity() {
            for (unsigned int i = 0; i < faces.size(); i++)
                delete faces[i];
            faces.clear();
        }
    }
}
