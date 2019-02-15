#ifndef ODFAEG_SHADOW_RENDER_COMPONENT_HPP
#define ODFAEG_SHADOW_RENDER_COMPONENT_HPP
#include "renderWindow.h"
#include "renderTexture.h"
#include "tile.h"
#include "entityManager.h"
#include "heavyComponent.h"
#include "2D/ambientLight.h"
#include "model.h"
#include "rectangleShape.h"
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
        class ODFAEG_GRAPHICS_API ShadowRenderComponent : public HeavyComponent {
            public :
                ShadowRenderComponent (RenderWindow& window, int layer, std::string expression,window::ContextSettings settings = window::ContextSettings(0, 0, 4, 3, 0));
                void drawNextFrame();
                std::vector<Entity*> getEntities();
                void draw(RenderTarget& target, RenderStates states);
                void draw(Drawable& drawable, RenderStates states) {
                }
                void pushEvent(window::IEvent event, RenderWindow& rw);
                bool needToUpdate();
                View& getView();
                int getLayer();
                const Texture& getStencilBufferTexture();
                const Texture& getShadowMapTexture();
                Tile& getFrameBufferTile ();
                Tile& getDepthBufferTile();
                void setExpression(std::string expression);
                std::string getExpression();
                void setView(View view);
                bool loadEntitiesOnComponent(std::vector<Entity*> vEntities);
                void clear();
                void changeVisibleEntities(Entity* toRemove, Entity* toAdd, EntityManager* em);
                void updateParticleSystems();
            private :
                Batcher batcher, shadowBatcher; /**> A group of faces using the same materials and primitive type.*/
                std::vector<Instance> m_instances; /**> Instances to draw. (Instanced rendering.) */
                std::vector<Instance> m_shadow_instances;
                std::vector<Entity*> visibleEntities; /**> Entities loaded*/
                std::unique_ptr<RenderTexture> stencilBuffer; /**> the stencil buffer.*/
                std::unique_ptr<RenderTexture> shadowMap; /**> the shadow map.*/
                std::unique_ptr<Tile>  stencilBufferTile, shadowTile; /**> the stencil and shadow map buffer.*/
                std::unique_ptr<Shader> buildShadowMapShader; /**> the shader to generate the stencil buffer.*/
                std::unique_ptr<Shader>  perPixShadowShader; /**> the shader to generate the shadow map.*/
                View view; /**> the view of the component.*/
                std::string expression;
                bool update;
         };
    }
}
#endif


