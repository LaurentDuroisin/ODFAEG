#ifndef ODFAEG_Z_SORTING_RENDER_COMPONENT_HPP
#define ODFAEG_Z_SORTING_RENDER_COMPONENT_HPP
#include "renderWindow.h"
#include "entityManager.h"
#include "heavyComponent.h"
#include "../Physics/particuleSystem.h"
/**
  *\namespace odfaeg
  * the namespace of the Opensource Development Framework Adapted for Every Games.
  */
namespace odfaeg {
    namespace graphic {
        /**
          * \file OITRenderComponent.h
          * \class OITRenderComponent
          * \author Duroisin.L
          * \version 1.0
          * \date 1/02/2014
          * \brief represent a component used to render the entities of a scene.
          */
        class ODFAEG_GRAPHICS_API ZSortingRenderComponent : public HeavyComponent {
            public :
                ZSortingRenderComponent (RenderWindow& window, int layer, std::string expression, EntityManager& scene);
                void loadShaders();
                void pushEvent(window::IEvent event, RenderWindow& rw);
                bool needToUpdate();
                void changeVisibleEntities(Entity* toRemove, Entity* toAdd, EntityManager* em);
                std::string getExpression();
                bool loadEntitiesOnComponent(std::vector<Entity*> vEntities);
                void cutIntersectingVA();
                void setView(View view);
                void setExpression(std::string expression);
                std::vector<Entity*> getEntities();
                void draw(RenderTarget& target, RenderStates states);
                View& getView();
                int getLayer();
                void clear() {
                }
                void drawNextFrame() {
                }
                void draw(Drawable& drawable, RenderStates states) {
                }
                void updateParticleSystems();
                void updateTransformMatrices();
                void updateSceneVertices();
            private :
                Batcher batcher;
                View view;
                std::vector<Instance> m_instances;
                std::vector<Entity*> visibleEntities;
                std::string expression;
                bool update;
                EntityManager& scene;
                Shader instancedRenderingShader, shader;
        };
    }
}
#endif
