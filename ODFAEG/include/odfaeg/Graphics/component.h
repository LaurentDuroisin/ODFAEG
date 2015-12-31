#ifndef ODFAEG_COMPONENT_HPP
#include "drawable.h"
#include "transformable.h"
#include "../Core/listener.h"
#include "renderWindow.h"
namespace odfaeg {
    namespace graphic {
        class Component : public Drawable, public Transformable {
        public :
            Component(RenderWindow& window, math::Vec3f position, math::Vec3f size, math::Vec3f origin, unsigned int priority=0)
            : Transformable(position, size, origin),
            listener(),
            priority(priority),
            window(window) {
                activateEventContext = true;
                visible = true;
                id = nbComponents;
                nbComponents++;
            }
            virtual void clear() = 0;
            virtual void pushEvent(sf::Event event) = 0;
            virtual void onUpdate(RenderWindow* window, sf::Event& event) {}
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
            unsigned int getPriority() {
                return priority;
            }
            virtual void onVisibilityChanged(bool visible);
            virtual void onEventContextActivated(bool activate);
            RenderWindow& getWindow() {
                return window;
            }
        private :
            RenderWindow& window;
            core::Listener listener;
            bool activateEventContext;
            bool visible;
            int id;
            static int nbComponents;
            unsigned int priority;
        };
    }
}
#define ODFAEG_COMPONENT_HPP
#endif // ODFAEG_COMPONENT
