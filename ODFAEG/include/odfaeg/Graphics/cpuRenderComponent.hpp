#ifndef CPU_FAST_RENDER_COMPONENT
#define CPU_FAST_RENDER_COMPONENT
#include "batcher.h"
#include "component.h"
#include "renderWindow.h"
#include "entity.h"
#include <SFML/Graphics/Image.hpp>
#include "../Physics/boundingPolyhedron.h"
#include "tile.h"
#include <CL/cl.hpp>
#include <fstream>
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
                void pushEvent(sf::Event event) {
                    getListener().pushEvent(event);
                }
                int orient2d(const math::Vec2f& a, const math::Vec2f& b, const math::Vec2f& c) {
                    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
                }
                ~CPURenderComponent() {
                    delete fbTile;
                }
                struct Edge {
                    // Dimensions of our pixel group
                    static const int stepXSize = 4;
                    static const int stepYSize = 4;
                    math::Vec3f oneStepX;
                    math::Vec3f oneStepY;
                    math::Vec3f init(const math::Vec2f& v0, const math::Vec2f& v1,
                               const math::Vec2f& origin);
                };


            private :
                Tile* fbTile;
                RenderWindow& window;
                View view;
                Batcher batcher;
                std::vector<Instance*> m_instances;
                std::vector<unsigned char> redBuffer;
                std::vector<unsigned char> blueBuffer;
                std::vector<unsigned char> greenBuffer;
                std::vector<unsigned char> alphaBuffer;
                sf::Image winFrameBuffer;
                Texture fbTexture;
                std::vector<float> depthBuffer;
                std::vector<Entity*> visibleEntities;
                math::Vec2f size;
                cl::Buffer credBuffer, cblueBuffer, cgreenBuffer, calphaBuffer, cdepthBuffer;
                cl::Buffer cvposXBuffer, cvposYBuffer, cvposZBuffer, cvposWBuffer;
                cl::Buffer vcvposXBuffer, vcvposYBuffer, vcvposZBuffer, vcvposWBuffer;
                cl::Buffer vcvcolorRedBuffer, vcvcolorBlueBuffer, vcvcolorGreenBuffer, vcvcolorAlphaBuffer, vcvTCUBuffer, vcvTCVBuffer;
                cl::Buffer cvCentersXBuffer, cvCentersYBuffer, cvCentersZBuffer, cvCentersWBuffer;
                cl::Buffer cvAvgColorRedBuffer, cvAvgColorBlueBuffer, cvAvgColorGreenBuffer, cvAvgColorAlphaBuffer,
                           cvAvgTCUBuffer, cvAvgTCVBuffer;
                cl::Buffer cvcolRedBuffer, cvcolGreenBuffer, cvcolBlueBuffer, cvcolAlphaBuffer;
                cl::Buffer cvtexcoorduBuffer, cvtexcoordvBuffer;
                cl::Buffer ctransfMatrixBuffer, cviewMatrixBuffer, cprojMatrixBuffer, ctexMatrixBuffer, cvpMatrixBuffer;
                cl::Buffer cnumIndexesBuffer, cbaseVerticesBuffer, cbaseIndexesBuffer, cvindexesBuffer;
                cl::Buffer ctposXBuffer, ctposYBuffer, ctposZBuffer, ctposWBuffer;
                cl::Buffer ctcolRedBuffer, ctcolBlueBuffer, ctcolGreenBuffer, ctcolAlphaBuffer;
                cl::Buffer ctTCUBuffer, ctTCVBuffer;
                cl::Kernel clkfragmentShader;
                cl::Kernel clkvertexShader;
                cl::Kernel clkgeometryShader;
                cl::Kernel clktesslationShader;
                cl::Context clcontext;
                std::vector<cl::Device> devices;
        };
    }
}
#endif
