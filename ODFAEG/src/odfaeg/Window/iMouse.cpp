#include "../../../include/odfaeg/Window/iMouse.hpp"
#include "../../../include/odfaeg/Window/x11Mouse.hpp"
#include "../../../include/odfaeg/Window/window.hpp"
namespace odfaeg {
    namespace window {
        bool IMouse::isButtonPressed (Button button) {
            #if defined(ODFAEG_SYSTEM_LINUX)
                return X11Mouse::isButtonPressed(button);
            #endif
        }
        sf::Vector2i IMouse::getPosition() {
            #if defined(ODFAEG_SYSTEM_LINUX)
                return X11Mouse::getPosition();
            #endif
        }
        sf::Vector2i IMouse::getPosition(const Window& window) {
            #if defined (SFML)
                return sf::Mouse::
            #else
                #if defined(ODFAEG_SYSTEM_LINUX)
                    return X11Mouse::getPosition(static_cast<const X11Window&>(*window.getImpl()));
                #endif
            #endif
        }
    }
}

