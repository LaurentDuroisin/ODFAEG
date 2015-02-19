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
            listener(useThread) {
                activateEventContext = true;
                visible = true;
                id = nbComponents;
                nbComponents++;
            }
            virtual void clear() = 0;
            void pushEvent(sf::Event event) {
                listener.pushEvent(event);
            }
            virtual void onUpdate(sf::Event& event) {}
            core::Listener& getListener() {
                return listener;
            }
            void setEventContextActivated(bool activateEventContext) {
                this->activateEventContext = activateEventContext;
                onEventContextActivated(activateEventContext);
            }
            bool isEventContextActivated() {
                return activateEventContext;
            }
            void setVisible (bool visible) {
                this->visible = visible;
                onVisibilityChanged(visible);
            }
            bool isVisible() {
                return visible;
            }
            int getId() {
                return id;
            }
            virtual void onVisibilityChanged(bool visible) {}
            virtual void onEventContextActivated(bool activate) {}
        private :
            core::Listener listener;
            bool activateEventContext;
            bool visible;
            int id;
            static int nbComponents;
        };
    }
}
#define ODFAEG_COMPONENT_HPP
#endif // ODFAEG_COMPONENT
