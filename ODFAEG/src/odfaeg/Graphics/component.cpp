#include "../../../include/odfaeg/Graphics/component.h"
namespace odfaeg {
    namespace graphic {
        int Component::nbComponents = 0;
        void Component::setEventContextActivated(bool activateEventContext) {
            this->activateEventContext = activateEventContext;
            onEventContextActivated(activateEventContext);
        }
        bool Component::isEventContextActivated() {
            return activateEventContext;
        }
        void Component::setVisible (bool visible) {
            onVisibilityChanged(visible);
            this->visible = visible;
        }
        bool Component::isVisible() {
            return visible;
        }
        void Component::processEvents() {
            listener.processEvents();
        }
        void Component::onVisibilityChanged(bool visible) {}
        void Component::onEventContextActivated(bool activate) {}
    }
}
