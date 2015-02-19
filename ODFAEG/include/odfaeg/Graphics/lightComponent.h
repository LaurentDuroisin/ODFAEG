#ifndef ODFAEG_LIGHT_COMPONENT
#define ODFAEG_LIGHT_COMPONENT
#include "component.h"
namespace odfaeg {
    namespace graphic {
        class LightComponent : public Component {
            public :
            LightComponent(math::Vec3f position, math::Vec3f size, math::Vec3f origin, bool useThread) :
                Component (position, size, origin, useThread) {

            }
            virtual void update() {}
        };
    }
}
#endif // ODFAEG_LIGHT_COMPONENT
