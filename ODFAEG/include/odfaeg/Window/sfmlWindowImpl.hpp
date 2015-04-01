#ifndef ODFAEG_SDLWINDOWIMPL
#define ODFAEG_SDLWINDOWIMPL
#include "iWindow.hpp"
#include <SFML/Window.hpp>
namespace odfaeg {
    namespace window {
        class SFMLWindowImpl : public IWindow {
        private :
            SFMLWindowImpl(unsigned int width, unsigned int height, std::string title);
            void create(unsigned int width, unsigned int height, std::string title);
            void setPosition(unsigned int x, unsigned int y);
            bool isOpen();
            bool pollEvent(IEvent& event);
            void close();
            void setVisible (bool visible);
            sf::Window window;
        };
    }
}
#endif // ODFAEG_SDLWINDOWIMPL

