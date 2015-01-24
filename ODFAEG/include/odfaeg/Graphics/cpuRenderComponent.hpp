#ifndef CPU_FAST_RENDER_COMPONENT
#define CPU_FAST_RENDER_COMPONENT
#include "batcher.h"
#include "component.h"
#include "renderWindow.h"
#include "entity.h"
#include <SFML/Graphics/Image.hpp>
#include "../Physics/boundingPolyhedron.h"
#include "tile.h"
namespace odfaeg {
    namespace graphic {
        class CPURenderComponent : public Component {
            public :
                CPURenderComponent(math::Vec3f position, math::Vec3f size, math::Vec3f origin, bool usethread, RenderWindow& window);
                void loadEntitiesOnComponent(std::vector<Entity*> entities);
                void drawNextFrame();
                void draw(RenderTarget& target, RenderStates states = RenderStates::Default);
                void clear();
                void raytrace();
            private :
                RenderWindow& window;
                View view;
                Batcher batcher;
                std::vector<Instance*> m_instances;
                std::vector<unsigned char> frameBuffer;
                sf::Image winFrameBuffer;
                Texture fbTexture;
                std::vector<math::Vec3f> depthBuffer;
                std::vector<Entity*> visibleEntities;
                math::Vec2f size;
        };
    }
}
#endif
