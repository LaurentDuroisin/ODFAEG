#ifndef ODFAEG_LIGHT_COMPONENT
#define ODFAEG_LIGHT_COMPONENT
#include "component.h"
namespace odfaeg {
    namespace graphic {
        class LightComponent : public Component {
            public :
            LightComponent(math::Vec3f position, math::Vec3f size, math::Vec3f origin, bool useThread, unsigned int priority = 0, Component* parent = nullptr) :
                Component (position, size, origin, useThread, priority), parent(parent) {

            }
            virtual void checkSubWindowEvents() = 0;
            virtual ~LightComponent() {}
            private :
            Component* parent;
        };
    }
}
#endif // ODFAEG_LIGHT_COMPONENT
