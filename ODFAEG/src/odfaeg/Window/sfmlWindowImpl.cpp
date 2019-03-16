#include "../../../include/odfaeg/Window/sfmlWindowImpl.hpp"
namespace odfaeg {
    namespace window {
        SFMLWindowImpl::SFMLWindowImpl(unsigned int width, unsigned int height, std::string title) {
            window.create(sf::VideoMode(width, height), title);
        }
        void SFMLWindowImpl::create(unsigned int width, unsigned int height, std::string title) {
            window.create(sf::VideoMode(width, height), title);
        }
        void SFMLWindowImpl::setPosition(unsigned int x, unsigned int y) {
            window.setPosition(sf::Vector2i(x, y));
        }
        bool SFMLWindowImpl::isOpen() {
            return window.isOpen();
        }
        bool SFMLWindowImpl::pollEvent(IEvent& event) {
            sf::Event sfmlevent;
            if (window.pollEvent(sfmlevent)) {
                event.type = sfmlevent.type;
                event.window.type = 0;
                event.window.timestamp = 0;
                event.window.windowID = 0;
                event.window.event = 0;
                event.window.data1 = sfmlevent.size.width;
                event.window.data2 = sfmlevent.size.height;
                event.text.type = 0;
                event.text.timestamp = 0;
                event.text.windowID = 0;
                event.text.text = sfmlevent.text.unicode;
                event.keyboard.type = 0;
                event.keyboard.timestamp = 0;
                event.keyboard.windowID = 0;
                event.keyboard.state = 0;
                event.keyboard.repeat = 0;
                event.keyboard.scancode = 0;
                event.keyboard.keycode = sfmlevent.key.code;
                event.keyboard.mod = 0;
                event.mouseMotion.type = 0;
                event.mouseMotion.timestamp = 0;
                event.mouseMotion.windowID = 0;
                event.mouseMotion.which = 0;
                event.mouseMotion.state = 0;
                event.mouseMotion.x = sfmlevent.mouseMove.x;
                event.mouseMotion.y = sfmlevent.mouseMove.y;
                event.mouseMotion.xrel = 0;
                event.mouseMotion.yrel = 0;
                event.mouseButton.type = 0;
                event.mouseButton.timestamp = 0;
                event.mouseButton.windowID = 0;
                event.mouseButton.which = sfmlevent.mouseButton.button;
                event.mouseButton.state = 0;
                event.mouseButton.clicks = 0;
                event.mouseButton.x = sfmlevent.mouseButton.x;
                event.mouseButton.y = sfmlevent.mouseButton.y;
                event.mouseWheel.type = 0;
                event.mouseWheel.timestamp = 0;
                event.mouseWheel.windowID = 0;
                event.mouseWheel.which = 0;
                event.mouseWheel.x = sfmlevent.mouseButton.x;
                event.mouseWheel.y = sfmlevent.mouseButton.y;
                event.mouseWheel.direction = sfmlevent.mouseWheel.delta;
                return true;
            }
            return false;
        }
        void SFMLWindowImpl::close() {
            window.close();
        }
        void SFMLWindowImpl::setVisible (bool visible) {
            window.setVisible(visible);
        }
    }
}
