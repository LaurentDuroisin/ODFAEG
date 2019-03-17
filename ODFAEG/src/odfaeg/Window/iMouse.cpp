#include "../../../include/odfaeg/Window/iMouse.hpp"
#include "../../../include/odfaeg/Window/x11Mouse.hpp"
#include "../../../include/odfaeg/Window/window.hpp"
#if defined (SFML)
#include "../../../include/odfaeg/Window/sfmlMouse.hpp"
#include "../../../include/odfaeg/Window/sfmlWindowImpl.hpp"
#endif
namespace odfaeg {
    namespace window {
        bool IMouse::isButtonPressed (Button button) {
            #if defined(SFML)
                return SFMLMouse::isButtonPressed(button);;
            #elif defined (ODFAEG_SYSTEM_LINUX)
                return X11Mouse::isButtonPressed(button);
            #endif
        }
        sf::Vector2i IMouse::getPosition() {
            #if defined(SFML)
                return SFMLMouse::getPosition();
            #elif defined (ODFAEG_SYSTEM_LINUX)
                return X11Mouse::getPosition();
            #endif
        }
        sf::Vector2i IMouse::getPosition(const Window& window) {
            #if defined (SFML)
                return SFMLMouse::getPosition(static_cast<const SFMLWindowImpl&>(*window.getImpl()));
            #elif defined(ODFAEG_SYSTEM_LINUX)
                return X11Mouse::getPosition(static_cast<const X11Window&>(*window.getImpl()));
            #endif
        }
    }
}

