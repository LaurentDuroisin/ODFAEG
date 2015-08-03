#include "../../../include/odfaeg/Core/inputSystem.h"
namespace odfaeg {
Listener& InputSystem::listener = getListener();
Listener& InputSystem::getListener() {
    if (&listener == nullptr) {
        Listener* lsn = new Listener();
        return *lsn;
    }
    return listener;
}
}
