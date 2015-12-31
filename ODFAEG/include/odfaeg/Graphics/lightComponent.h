#ifndef ODFAEG_LIGHT_COMPONENT
#define ODFAEG_LIGHT_COMPONENT
#include "component.h"
namespace odfaeg {
    namespace graphic {
        class LightComponent : public Component {
            public :
            LightComponent(RenderWindow& window, math::Vec3f position, math::Vec3f size, math::Vec3f origin, unsigned int priority = 0, LightComponent* parent = nullptr) :
                Component (window, position, size, origin, priority), parent(parent) {

            }
            void setParent(LightComponent* parent) {
                this->parent = parent;
            }
            void setRelToParentLayout(math::Vec3f layout) {
                float sx, sy, npx, npy, nsx, nsy;
                sx = getSize().x;
                sy = getSize().y;
                if (parent != nullptr) {
                    npx = parent->getPosition().x + parent->getSize().x * layout.x;
                    npy = parent->getPosition().y + parent->getSize().y * layout.y;
                    nsx = parent->getSize().x * layout.z;
                    nsy = parent->getSize().y * layout.w;
                } else {
                    npx = getWindow().getSize().x * layout.x;
                    npy = getWindow().getSize().y * layout.y;
                    nsx = getWindow().getSize().x * layout.z;
                    nsy = getWindow().getSize().y * layout.w;
                }
                setScale(math::Vec3f(nsx / sx, nsy / sy, 1.f));
                setPosition(math::Vec3f(npx, npy, getPosition().z));
            }
            virtual void checkSubWindowEvents() = 0;
            virtual ~LightComponent() {}
            private :
            LightComponent* parent;
        };
    }
}
#endif // ODFAEG_LIGHT_COMPONENT
