#include "../../../../include/odfaeg/Graphics/3D/entity.h"

namespace odfaeg {
    namespace g3d {


int Entity::nbEntities = 0;
int Entity::nbEntitiesTypes = 0;

std::map<int, std::string>* Entity::types = initTypes();
Entity::Entity (Vec3f position, Vec3f size, Vec3f origin, std::string sType, Entity *parent) :
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
    collisionVolume = nullptr;
}
int Entity::getNbEntities () {
    return nbEntities;
}
void Entity::draw (RenderTarget &target, RenderStates states) {
    states.transform = getTransform();
    //std::cout<<states.transform.getMatrix()[12]<<" "<<states.transform.getMatrix()[13]<<std::endl;
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
    children.push_back(child);
}
void Entity::removeChild (Entity *child) {
    std::vector<Entity*>::iterator it;
    for (it = children.begin(); it != children.end(); it++) {
        if (*it == child)
            it = children.erase(it);
        else
            it++;
    }
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

void Entity::onMove(Vec3f &t) {
    for (unsigned int i = 0; i < children.size(); i++)
        children[i]->move(t);
}
void Entity::onScale(Vec3f &s) {
    for (unsigned int i = 0; i < children.size(); i++)
        children[i]->scale(s);
}
void Entity::onRotate(float angle) {
    for (unsigned int i = 0; i < children.size(); i++)
        children[i]->rotate(angle);
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
    nbEntities--;
}
}
}
