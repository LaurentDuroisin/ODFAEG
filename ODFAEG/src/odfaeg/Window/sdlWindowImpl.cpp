#include "../../../include/odfaeg/Window/sdlWindowImpl.hpp"
namespace odfaeg {
    namespace window {
        SDLWindowImpl::SDLWindowImpl(unsigned int width, unsigned int height, std::string title)
        {
            window.create(width, height, title);
        }
        void SDLWindowImpl::create(unsigned int width, unsigned int height, std::string title) {
            window.create(width, height, title);
        }
        void SDLWindowImpl::setPosition(unsigned int x, unsigned int y) {
            window.setPosition(x, y);
        }
        bool SDLWindowImpl::isOpen() {
            return window.isOpen();
        }
        bool SDLWindowImpl::pollEvent(IEvent& event) {
            SDL_Event sdlevent;
            if (window.pollEvent(sdlevent)) {
                event.type = sdlevent.type;
                event.window.type = sdlevent.window.type;
                event.window.timestamp = sdlevent.window.timestamp;
                event.window.event = sdlevent.window.event;
                event.window.windowID = sdlevent.window.windowID;
                event.window.data1 = sdlevent.window.data1;
                event.window.data2 = sdlevent.window.data2;
                event.text.type = sdlevent.text.type;
                event.text.timestamp = sdlevent.text.timestamp;
                event.text.windowID = sdlevent.text.windowID;
                event.text.text = reinterpret_cast<long long int>(sdlevent.text.text);
                event.keyboard.type = sdlevent.key.type;
                event.keyboard.timestamp = sdlevent.key.timestamp;
                event.keyboard.windowID = sdlevent.key.windowID;
                event.keyboard.state = sdlevent.key.state;
                event.keyboard.repeat = sdlevent.key.repeat;
                event.keyboard.scancode = sdlevent.key.keysym.scancode;
                event.keyboard.keycode = sdlevent.key.keysym.sym;
                event.keyboard.mod = sdlevent.key.keysym.mod;
                event.mouseMotion.type = sdlevent.motion.type;
                event.mouseMotion.timestamp = sdlevent.motion.timestamp;
                event.mouseMotion.windowID = sdlevent.motion.windowID;
                event.mouseMotion.which = sdlevent.motion.which;
                event.mouseMotion.state = sdlevent.motion.state;
                event.mouseMotion.x = sdlevent.motion.x;
                event.mouseMotion.y = sdlevent.motion.y;
                event.mouseMotion.xrel = sdlevent.motion.xrel;
                event.mouseMotion.yrel = sdlevent.motion.yrel;
                event.mouseButton.type = sdlevent.button.type;
                event.mouseButton.timestamp = sdlevent.button.timestamp;
                event.mouseButton.windowID = sdlevent.button.windowID;
                event.mouseButton.which = sdlevent.button.which;
                event.mouseButton.state = sdlevent.button.state;
                event.mouseButton.clicks = sdlevent.button.clicks;
                event.mouseButton.x = sdlevent.button.x;
                event.mouseButton.y = sdlevent.button.y;
                event.mouseWheel.type = sdlevent.wheel.type;
                event.mouseWheel.timestamp = sdlevent.wheel.timestamp;
                event.mouseWheel.windowID = sdlevent.wheel.windowID;
                event.mouseWheel.which = sdlevent.wheel.which;
                event.mouseWheel.x = sdlevent.wheel.x;
                event.mouseWheel.y = sdlevent.wheel.y;
                //event.mouseWheel.direction = sdlevent.wheel.direction;
                return true;
            }
            return false;
        }
        void SDLWindowImpl::close() {
            window.close();
        }
        void SDLWindowImpl::setVisible (bool visible) {
            window.setVisible(visible);
        }
    }
}
