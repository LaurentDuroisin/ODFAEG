#include "../../../include/odfaeg/Window/iKeyboard.hpp"
#include "../../../include/odfaeg/Window/x11Keyboard.hpp"
namespace odfaeg {
    namespace window {
        bool IKeyboard::isKeyPressed(Key key) {
            #if defined(ODFAEG_SYSTEM_LINUX)
                return X11Keyboard::isKeyPressed(key);
            #elif defined (SFML)
                return SFMLKeyboard::isKeyPressed(key);
            #endif
        }
    }
}
