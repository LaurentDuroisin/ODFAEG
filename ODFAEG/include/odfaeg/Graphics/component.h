#ifndef ODFAEG_COMPONENT_HPP
#include "drawable.h"
#include "transformable.h"
#include "../Core/listener.h"
namespace odfaeg {
    namespace graphic {
        class Component : public Drawable, public Transformable {
        public :
            Component(math::Vec3f position, math::Vec3f size, math::Vec3f origin, bool useThread)
            : Transformable(position, size, origin),
            listener(useThread) {}
            virtual void clear() = 0;
            void pushEvent(sf::Event event) {
                listener.pushEvent(event);
            }
            core::Listener& getListener() {
                return listener;
            }
        protected :
            core::Listener listener;
        };
    }
}
#define ODFAEG_COMPONENT_HPP
#endif // ODFAEG_COMPONENT
