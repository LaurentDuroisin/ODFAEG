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
            virtual void pushEvent(sf::Event event) = 0;
            virtual void onUpdate(sf::Event& event) {}
            core::Listener& getListener() {
                return listener;
            }
            void setEventContextActivated(bool activateEventContext);
            bool isEventContextActivated();
            void setVisible (bool visible);
            bool isVisible();
            int getId() {
                return id;
            }
            virtual void onVisibilityChanged(bool visible);
            virtual void onEventContextActivated(bool activate);
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
