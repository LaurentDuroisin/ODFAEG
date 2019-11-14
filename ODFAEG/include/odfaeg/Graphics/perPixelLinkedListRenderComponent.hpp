#ifndef ODFAEG_PERPIXEL_LINKEDLIST_RENDER_COMPONENT_HPP
#define ODFAEG_PERPIXEL_LINKEDLIST_RENDER_COMPONENT_HPP
#include "heavyComponent.h"
#include "renderTexture.h"
#include "sprite.h"
#include "rectangleShape.h"
namespace odfaeg {
    namespace graphic {
        /*struct ListNode {
          math::Vec3f color;
          GLfloat depth;
          GLuint next;
        };*/
        class PerPixelLinkedListRenderComponent : public HeavyComponent {
            public :
            PerPixelLinkedListRenderComponent (RenderWindow& window, int layer, std::string expression, window::ContextSettings settings);
                 /**
            * \fn bool loadEntitiesOnComponent(std::vector<Entity*> visibleEntities)
            * \brief load the given entities onto the component.
            * \param std::vector<Entity*> visibleEntities : the entities to load.
            * \return if the loading was sucessfull.
            */
            std::vector<Entity*> getEntities();
            bool loadEntitiesOnComponent(std::vector<Entity*> visibleEntities);
            bool needToUpdate();
            /**
            * \fn void clearBufferBits()
            * \brief clear the buffer bits of the component.
            */
            void clear ();
            /**
            * \fn void setBackgroundColor(sf::Color color)
            * \brief set the background color of the component. (TRansparent by default)
            * \param sf::Color color : the color.
            */
            void setBackgroundColor(sf::Color color);
            /**
            * \fn void drawNextFrame()
            * \brief draw the next frame of the component.
            */
            void drawNextFrame();
            void setExpression (std::string expression);
            /**
            * \fn draw(Drawable& drawable, RenderStates states = RenderStates::Default);
            * \brief draw a drawable object onto the component.
            * \param Drawable drawable : the drawable object to draw.
            * \param RenderStates states : the render states.
            */
            void draw(Drawable& drawable, RenderStates states = RenderStates::Default);
            /**
            * \fn void draw(RenderTarget& target, RenderStates states)
            * \brief draw the frame on a render target.
            * \param RenderTarget& target : the render target.
            * \param RenderStates states : the render states.
            */
            void draw(RenderTarget& target, RenderStates states);
            std::string getExpression();
            /**
            * \fn int getLayer()
            * \brief get the layer of the component.
            * \return the number of the layer.
            */
            int getLayer();
            /**
            * \fn void setView(View& view)
            * \brief set the view of the component.
            * \param the view of the component.
            */
            void changeVisibleEntities(Entity* toRemove, Entity* toAdd, EntityManager* em);
            /**
            * \fn register an event to the event stack of the component.
            * \param window::IEvent : the event to register.
            * \param Renderwindow : the window generating the event.
            */
            void pushEvent(window::IEvent event, RenderWindow& window);
            void updateParticleSystems();
            void setView(View view);
            View& getView();
            ~PerPixelLinkedListRenderComponent();
            private :
            RectangleShape quad;
            std::vector<std::pair<std::reference_wrapper<Drawable>, RenderStates>> drawables;
            Batcher batcher; /**> A group of faces using the same materials and primitive type.*/
            sf::Color backgroundColor; /**> The background color.*/
            std::vector<Instance> m_instances; /**> Instances to draw. (Instanced rendering.) */
            std::vector<std::unique_ptr<Face>> additionalFaces;
            std::vector<Entity*> visibleEntities; /**> Entities loaded*/
            RenderTexture frameBuffer; /**> the frame buffer.*/
            Shader perPixelLinkedList, perPixelLinkedListP2, filterNotOpaque, initialize;
            RenderStates currentStates; /**> the current render states.*/
            View view; /**> the view of the component.*/
            std::string expression;
            bool update;
            unsigned int atomicBuffer, linkedListBuffer, clearBuf, clearBuf2, clearBuf3, pass1Index, pass2Index, headPtrTex, colorTex, depthTex;
            Sprite frameBufferSprite;
            static int nb_created;
        };
    }
}
#endif // ODFAEG_PERPIXEL_LINKEDLIST_RENDER_COMPONENT_HPP
