#ifndef ODFAEG_ENTITY_3D
#define ODFAEG_ENTITY_3D

#include "../transformable.h"
#include "../drawable.h"
#include "../face.h"
#include "../../Physics/boundingBox.h"
#include "../../Core/state.h"
#include "../../Core/stateExecutor.h"
#include "../renderTarget.h"

#include <vector>

namespace odfaeg {
    namespace g3d {

/**Class entity by Laurent Duroisin (all right reserved)
 * this abstract class is the base of all drawable and transformable's entities of the framework.
 * this class inherits from the transformable and the drawable class.
 * entities are components, then, they can have parent entities and children entities.
 * (CF : Design pattern component)
 */
class Entity : public Transformable, public Drawable {

    public :
        /** position : the position of the top left corner of the entity
         * size : the size of the entity
         * origin : the origin of the entity's position wich is local to the entity's position.
         * zOrder : the layer's position of the entity
         * type : the type of the entity. (the type can be anything (model, floor, light, shadow, etc...))
         * the type should describe the kind of the entity.
         * parent : the parent of the entity, the value is null if the entity doesn't have a parent.
         */
        Entity (Vec3f position, Vec3f size, Vec3f origin, std::string type, Entity* parent = nullptr);
        //Get the number of entities created with the application.
        static int getNbEntities ();
        //Get teh type of the entity.
        std::string getType() const;
        //Get the type's id of the entity.
        int getTypeInt ();
        //Get the type corresponding to the given id.
        static std::string getTypeOfInt (int type);
        //Get the id corresponding to the given type.
        static int getIntOfType (std::string type);
        //Get the radius of the entity.
        int getRadius ();
        //Init the map who is a correspondance between a type of an entity and the id of it's type.
        static std::map<int, std::string>* initTypes () {
            if (types == nullptr) {
                static std::map<int, std::string> *t = new std::map<int, std::string> ();
                return t;
            }
            return types;
        }
        //Return the id of the entity.
        int& getId();
        //Set an id to the entity.
        void setId (int id);
        //This method have to be redifinied to determine when two entity's are equals.
        //This can avoid to have two equqls entity's at the same place.
        virtual bool operator== (Entity& other) = 0;
        void draw (RenderTarget &target, RenderStates states);
        virtual void onDraw(RenderTarget &target, RenderStates states) {}
        virtual void onMove(Vec3f &t);
        virtual void onScale(Vec3f &s);
        virtual void onRotate(float angle);
        //Return the parent of the entity.
        Entity* getParent() const;
        //Set the parent's entity of the entity.
        void setParent (Entity *parent);
        //Add a children to the entity.
        void addChild (Entity *child);
        //Remove a children to the entity.
        void removeChild (Entity *child);
        //Return the children of the entities.
        std::vector<Entity*> getChildren() const;

        //Return the number of entity's children.
        unsigned int getNbChildren ();
        //Determine if the entity is animated.
        virtual bool isAnimated() const = 0;
        //Determine if the entity have a shadow.
        virtual bool isModel() const = 0;
        virtual bool selectable() const = 0;
        //Set the type of the entity.
        void setType (std::string type);
        //Destructor.
        template <typename T> bool addAttribute (std::string name, T value) {
            return entityState.addParameter(name, value);
        }
        template <typename T> bool addAttribute (std::string name, T* value) {
            return entityState.addParameter(name, value);
        }
        const StateParameter& getAttribute (const std::string name) throw (odfaeg::Erreur) {
               return entityState.getParameter(name);
        }
        template <typename T> void changeAttribute (const std::string name, T value) {
           entityState.changeParameter(name, value);
        }
        bool removeAttribute (std::string name) {
            entityState.removeParameter(name);
        }
        bool interact(StateExecutor *se) {
            entityState.setExecutor(se);
            return entityState.doActionState();
        }
        bool uninteract (StateExecutor *se) {
            entityState.setExecutor(se);
            return entityState.undoActionState();
        }
        void setCollisionVolume (BoundingVolume* boundingVolume) {
            this->collisionVolume = boundingVolume;
        }
        BoundingVolume* getCollisionVolume () {
            return collisionVolume;
        }
        void addFace(Face* face);
        std::vector<Face*> getFaces() const;
        virtual ~Entity();
    private :
        static std::map<int, std::string> *types;
        std::pair<int, std::string> type;
        std::vector<Entity*> children;
        std::vector<Face*> faces;
        Entity* parent;
        int id;
        static int nbEntities, nbEntitiesTypes;
        State entityState;
        BoundingVolume* collisionVolume;
};
}
}
#endif
