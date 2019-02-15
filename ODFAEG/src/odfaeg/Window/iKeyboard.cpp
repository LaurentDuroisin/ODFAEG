#include "../../../include/odfaeg/Window/iKeyboard.hpp"
namespace odfaeg {
    namespace window {
        bool IKeyboard::isKeyPressed(Key key) {
            #if defined(ODFAEG_SYSTEM_LINUX)
                return X11Keyboard::isKeyPressed(key);
            #endif
        }
    }
}
