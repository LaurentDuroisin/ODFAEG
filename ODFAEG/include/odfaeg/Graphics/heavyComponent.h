#ifndef ODFAEG_HEAVY_COMPONENT
#define ODFAEG_HEAVY_COMPONENT
#include "component.h"
#include "entity.h"
namespace odfaeg {
    namespace graphic {
        class EntityManager;
        class HeavyComponent : public Component {
            public :
            HeavyComponent(RenderWindow& window, math::Vec3f position, math::Vec3f size, math::Vec3f origin) :
                Component(window, position, size, origin) {}
            void recomputeSize() {
                std::cout<<"recompute size!"<<std::endl;
                float sx, sy, npx, npy, nsx, nsy;
                sx = getSize().x;
                sy = getSize().y;
                npx = getWindow().getSize().x * getRelPosition().x;
                npy = getWindow().getSize().y * getRelPosition().y;
                nsx = getWindow().getSize().x * getRelSize().x;
                nsy = getWindow().getSize().y * getRelSize().y;
                setScale(math::Vec3f(nsx / sx, nsy / sy, 1.f));
                setPosition(math::Vec3f(npx, npy, getPosition().z));
            }
            virtual bool loadEntitiesOnComponent(std::vector<Entity*> entity) = 0;
            virtual std::vector<Entity*> getEntities() = 0;
            virtual std::string getExpression() = 0;
            virtual void setExpression(std::string expression) = 0;
            virtual void changeVisibleEntities(Entity* toRemove, Entity* toAdd, EntityManager* em) = 0;
            virtual void drawNextFrame() = 0;
            virtual void draw(Drawable& drawable, RenderStates states) = 0;
            virtual void setView(View view) = 0;
            virtual View& getView() = 0;
            /**
            * \fn const Texture& getFrameBufferTexture()
            * \brief get the texture of the frame buffer.
            * \return the texture of the framebuffer.
            */
            virtual const Texture& getFrameBufferTexture() = 0;
            /**
            * \fn const Texture& getDepthBufferTexture()
            * \brief get the texture of the depth buffer.
            * \return the texture of the depthbuffer.
            */
            virtual const Texture& getDepthBufferTexture() = 0;
            /**
            * \fn const Texture& getSpecularTexture()
            * \brief get the texture of the specular buffer.
            * \return the texture with the specular components.
            */
            virtual const Texture& getSpecularTexture() = 0;
            virtual const Texture& getRefractionTexture() = 0;
            virtual const Texture& getBumpTexture() = 0;
            virtual Tile& getFrameBufferTile() = 0;
            virtual Tile& getDepthBufferTile() = 0;
        };
    }
}
#endif // ODFAEG_HEAVY_COMPONENT
