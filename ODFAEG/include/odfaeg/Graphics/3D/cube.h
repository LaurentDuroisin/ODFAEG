#ifndef CUBE
#define CUBE
#include <SFML/Graphics/Color.hpp>
#include "../vertexArray.h"
#include "../entity.h"
namespace odfaeg {
    namespace graphic {
        namespace g3d {
            class ODFAEG_GRAPHICS_API Cube : public Entity {
            public :
                Cube (math::Vec3f position, float w, float h, float d, sf::Color color);
                bool operator== (Entity& other) {
                    if (other.getType() != "E_CUBE")
                        return false;
                    Cube &cube = static_cast<Cube&>(other);
                    return m_color == cube.m_color;
                }
                bool isAnimated() const {
                    return false;
                }
                bool isModel() const {
                    return false;
                }
                bool selectable() const {
                    return false;
                }
                bool isLight() const {
                    return false;
                }
                bool isShadow() const {
                    return false;
                }
                bool isLeaf() const {
                    return true;
                }
                void onDraw(RenderTarget &target, RenderStates states) const;
            private:
                sf::Color m_color;
            };
        }
    }
}
#endif // CUBE
