#ifndef ODFAEG_ENTITY_2D_HPP
#define ODFAEG_ENTITY_2D_HPP
#include "transformable.h"
#include "drawable.h"
#include "../Core/state.h"
#include "../Core/stateExecutor.h"
#include "renderTarget.h"
#include "batcher.h"
#include "../Math/computer.h"
#include "../Core/serialization.h"
#include <vector>
/**
  *\namespace odfaeg
  * the namespace of the Opensource Development Framework Adapted for Every Games.
  */
namespace odfaeg {
    namespace graphic {
        /**
          * \file entity.h
          * \class Entity
          * \brief Represent a 2D entity.
          * \author Duroisin.L
          * \version 1.0
          * \date 1/02/2014
          * \brief Abstract class which can represent any type of entities.
          *  Entities can have 0, 1 or more children and 0 or one parent.
          *  Each entity of the framework inherits from this class.
          */
        class ODFAEG_GRAPHICS_API Entity : public Transformable, public Drawable, public core::Registered<Entity> {
            public :
                /** \fn Entity(math::Vec3f position, math::Vec3f size, math::Vec3f origin, std::string type, Entity* parent)
                 * \brief constructor.
                 * \param position : the position of the top left corner of the entity
                 * \param size : the size of the entity
                 * \param origin : the origin of the entity's position wich is local to the entity's position.
                 * \param zOrder : the layer's position of the entity
                 * \param type : the type of the entity. (the type can be anything (model, floor, light, shadow, etc...))
                 * \param the type should describe the kind of the entity.
                 * \param parent : the parent of the entity, the value is null if the entity doesn't have a parent.
                 */
                Entity (math::Vec3f position, math::Vec3f size, math::Vec3f origin, std::string type, Entity* parent = nullptr);
                //Get the number of entities created with the application.
                /**
                *  \fn int getNbEntities ()
                *  \brief get the number of entities which exist.
                *  \return the number of entities which exist.
                */
                static int getNbEntities ();
                //Get teh type of the entity.
                /**
                *  \fn std::string getType() const;
                *  \brief return the type's/group name of the entity.
                *  \return the type/group name of the entity.
                */
                std::string getType() const;
                //Get the type's id of the entity.
                /**
                *  \fn int getTypeInt()
                *  \brief get the type/group id of the entity
                *  \return the type/group id of the entity.
                */
                int getTypeInt ();
                //Get the type corresponding to the given id.
                /**
                *  \fn std::string getTypeOfInt(int type)
                *  \brief get the type's name of the given type's id.
                *  \param the type's id.
                *  \return  int type : the type's name of the given type's id.
                */
                static std::string getTypeOfInt (int type);
                /**
                *  \fn int getIntOfType (std::string type)
                *  \brief get the type's id of the given type's name.
                *  \param get the type's id of the given type's name.
                *  \param the type's name.
                *  \return the type's id.
                */
                //Get the id corresponding to the given type.
                static int getIntOfType (std::string type);
                /**
                *  \fn int getRadius()
                *  \brief get the radius of the entity.
                *  \return int the radius of the entity.
                */
                //Get the radius of the entity.
                int getRadius ();
                /** \fn std::string getRootType.
                *   \param get the type of the root entity of the scene graph.
                */
                std::string getRootType();
                //Init the map who is a correspondance between a type of an entity and the id of it's type.
                /** \fn std::string std::map<int, std::string>
                *   \param get the list of the id's and associated type's names.
                */
                static std::map<int, std::string>* initTypes () {
                    if (types == nullptr) {
                        static std::map<int, std::string> *t = new std::map<int, std::string> ();
                        return t;
                    }
                    return types;
                }
                //Return the id of the entity.
                /** \fn int getId()
                *   \brief get the id of the entity.
                *   \return the id of the entity.
                */
                int& getId();
                //Set an id to the entity.
                 /** \fn void setId(int id)
                *   \brief set the id of the entity.
                *   \param the id of the entity.
                */
                void setId (int id);
                //This method have to be redifinied to determine when two entity's are equals.
                //This can avoid to have two equqls entity's at the same place.
                /** \fn
                *   \brief abstract method to redefine to compare two entities.
                *   \return if two entities are equals.
                */
                virtual bool operator== (Entity& other) = 0;
                /** \fn
                *   \brief draw (RenderTarger& target, RenderStates states)
                *   \param RenderTarget target : the rendertarget onwhich to draw the entity.
                *   \param RenderStates states : the states used to render the entities.
                */
                void draw (RenderTarget &target, RenderStates states);
                /** \fn
                *   \brief void onDraw (RenderTarger& target, RenderStates states)
                *   \param RenderTarget target : the rendertarget onwhich to draw the entities.
                *   \param RenderStates states : the states used to render the entities.
                */
                virtual void onDraw(RenderTarget &target, RenderStates states) const {}
                /** \fn void onMove (math::Vec3f& t)
                *   \brief virtual method which can be redefined if we need to do something when the entity is moving.
                *   \param math::Vec3f t : the translation of the entity.
                */
                virtual void onMove(math::Vec3f &t);
                /** \fn void onScale (math::Vec3f& s)
                *   \brief virtual method which can be redefined if we need to do something when the entity is rescaling.
                *   \param math::Vec3f& s : the scale of the entity.
                */
                virtual void onScale(math::Vec3f &s);
                /** \fn void onRotate (float angle)
                *   \brief virtual method which can be redefined if we need to do something when the entity is rotating.
                *   \param math::Vec3f& s : the rotation angle of the entity.
                */
                virtual void onRotate(float angle);
                //Return the parent of the entity.
                /** \fn Entity* getParent()
                *   \brief Entity* getParent() const;
                *   \return the parent of the entity.
                */
                Entity* getParent() const;
                //Set the parent's entity of the entity.
                /** \fn void setParent(Entity* parent)
                *   \brief set the parent of the entity.
                *   \param the parent of the entity.
                */
                void setParent (Entity *parent);
                //Add a children to the entity.
                /** \fn void addChild(Entity* child)
                *   \brief add a child to the entity.
                *   \param Entity* the entity to add.
                */
                void addChild (Entity *child);
                //Remove a children to the entity.
                /** \fn void removeChild(Entity* child)
                *   \brief remove a child from the entity.
                */
                void removeChild (Entity *child);
                //Return the children of the entities.
                /** \fn std::vector<Entity*> getChildren() const;
                *   \brief get the list of the children of the entities.
                *   \return std::vector<Entity*> get the entities.
                */
                std::vector<Entity*> getChildren() const;
                //Return the number of entity's children.
                /** \fn  unsigned int getNbChildren ();
                *   \brief get the number of children of the entity.
                *   \return the number of children of the entity.
                */
                unsigned int getNbChildren ();
                //Determine if the entity is animated.
                /** \fn  bool isAnimated ();
                *   \brief virtual method to redefine if an entity is animated.
                *   \return if the entity is animated.
                */
                virtual bool isAnimated() const = 0;
                //Determine if the entity have a shadow.
                /**
                * \fn bool isModel() const;
                * \brief virtual method to redefine if an entity is a model.
                * \return if the entity is a model.
                */
                virtual bool isModel() const = 0;
                /**
                * \fn bool isSelectable() const;
                * \brief virtual method to redefine if an entity is selectable.
                * \return if the entity is selectable.
                */
                virtual bool selectable() const = 0;
                /**
                * \fn bool isLight() const;
                * \brief virtual method to redefine if an entity is a light.
                * \return if the entity is a light.
                */
                virtual bool isLight() const = 0;
                /**
                * \fn bool isShadow() const;
                * \brief virtual method to redefine if an entity is a shadow.
                * \return if the entity is a shadow.
                */
                virtual bool isShadow() const = 0;
                /**
                * \fn bool isLeaf() const;
                * \brief virtual method to redefine if an entity is a leaf. (the entity have no children)
                * \return if the entity is a leaf.
                */
                virtual bool isLeaf() const = 0;
                //Set the type of the entity.
                /** \fn void setType (std::string type)
                *   \brief set the type's name of the entity.
                *   \param type : the type's name of the entity.
                */
                void setType (std::string type);
                //Destructor.
                /** \fn void addAttribute (std::string name, T value)
                *   \brief add an attribute to the entity.
                *   \param std::string the name of the attribute.
                *   \param T the value of the attribute.
                *   \return if the attribute has been correctly added.
                */
                template <typename T> bool addAttribute (std::string name, T value) {
                    return entityState.addParameter(name, value);
                }
                template <typename T> bool addAttribute (std::string name, T* value) {
                    return entityState.addParameter(name, value);
                }
                /** \fn const StateParameter& getAttribute (const std::string name)
                *   \brief get an attribute of the entity.
                *   \param std::string name : the name of the attribute of the entity.
                *   \return StateParameter& : the attribute of the entity.
                */
                const core::StateParameter& getAttribute (const std::string name) throw (core::Erreur) {
                       return entityState.getParameter(name);
                }
                /** \fn void changeAttribute (const std::string name)
                *   \brief change the value of the attribute of the entity.
                *   \param std::string name : the name of the attribute of the entity.
                *   \param T value : the value of the attribute.
                */
                template <typename T> void changeAttribute (const std::string name, T value) {
                    entityState.changeParameter(name, value);
                }
                /** \fn void removeAttribute (const std::string name)
                *   \brief remove an attribute from the entity.
                *   \return bool if the entity has been correctly removed.
                */
                bool removeAttribute (std::string name) {
                    entityState.removeParameter(name);
                    return true;
                }
                /** \fn bool interact (StateExecutor *se)
                *   \brief apply a state to the entity.
                *   \return bool if the state has been successfully applied.
                */
                bool interact(core::StateExecutor* se) {
                    entityState.setExecutor(se);
                    return entityState.doActionState();
                }
                /** \fn bool uninteract (StateExecutor *se)
                *   \brief unapply a state to the entity.
                *   \return bool if the state has been successfully applied.
                */
                bool uninteract (core::StateExecutor* se) {
                    entityState.setExecutor(se);
                    return entityState.undoActionState();
                }
                /** \fn void setCollisionVolumme (BoundingVolume* volume)
                *   \brief set the collision volume to the entity.
                *   \param BoundingVolume* volume : the collision volume to set.
                */
                void setCollisionVolume (physic::BoundingVolume* volume) {
                    collisionVolume.reset(volume);
                }
                /** \fn BoundingVolume* getCollisionVolumme ()
                *   \brief get the collision volume of the entity.
                *   \return the collision volume of the entity.
                */
                physic::BoundingVolume* getCollisionVolume () {
                   return collisionVolume.get();
                }
                /** \fn void addFace(Face* face)
                *   \brief add a face to the entity.
                */
                void addFace(Face* face);
                /** \fn std::vector<Face*> getFaces()
                *   \brief get the faces of the entity.
                *   \return std::vector<Face*> the faces of the entity.
                */
                std::vector<Face*> getFaces() const;
                /** \fn void vtserialize(Archive & ar)
                *   \brief serialize the entity into an archive.
                *   \param Archive : the archive onwhich to serialize the entities.
                */
                void updateTransform();
                template <typename Archive>
                void vtserialize(Archive & ar) {
                    /*if (!alreadySerialized) {
                        std::cout<<"write parent"<<std::endl;
                        ar(parent);
                        alreadySerialized = true;
                    }*/
                    Transformable::serialize(ar);
                    ar(id);
                    ar(faces);
                    ar(type.first);
                    ar(type.second);
                    ar(collisionVolume);
                    if (ar.isInputArchive())
                        onLoad();
                    ar(children);
                }
                /** \fn void onLoad()
                *   \brief load the entities.
                */
                void onLoad() {
                    types = initTypes();
                    int iType = getIntOfType(getType());
                    if (iType == -1) {
                        type = std::pair<int, std::string> (nbEntitiesTypes, getType());
                        types->insert(type);
                        nbEntitiesTypes++;
                    }
                    nbEntities++;
                }
                Entity* getRoot() {
                    if (parent != nullptr)
                        return parent->getRoot();
                    return this;
                }
                void getCombinedTransform(TransformMatrix& tm) {
                    if (parent != nullptr)
                        parent->getCombinedTransform(tm);
                    tm.combine(getTransform().getMatrix());
                }
                /** \fn virtual ~Entity();
                *   \brief destructor.
                */
                virtual ~Entity();
            protected :
                std::vector<Face*> faces; /**> the faces of the entity.*/
                static std::map<int, std::string> *types; /** A list of the type's id and name's of the entities. */
                std::pair<int, std::string> type; /** The type's id and the type's name of the entity.*/
                std::vector<Entity*> children; /** the children of the entities. */
                Entity* parent; /** the parent of the entity. */
                int id; /** the id of the entity.*/
                static int nbEntities, nbEntitiesTypes; /** the number of entities and the number of entities types.*/
                core::State entityState; /** the states of the entity.*/
                std::unique_ptr<physic::BoundingVolume> collisionVolume; /** the collision volume of the entity*/
                Entity(const Entity& entity) = delete; /**> an entity if not copiable.*/
                Entity& operator=(const Entity& entity) = delete; /**> an entity is not affectable*/
                bool alreadySerialized;
        };
    }
}
#endif
