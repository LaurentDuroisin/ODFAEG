#ifndef ODFAEG_SDLWINDOWIMPL
#define ODFAEG_SDLWINDOWIMPL
#include "iWindow.hpp"
#include "sdlWindow.hpp"
namespace odfaeg {
    namespace window {
        class SDLWindowImpl : public IWindow {
        private :
            SDLWindowImpl(unsigned int width, unsigned int height, std::string title);
            void create(unsigned int width, unsigned int height, std::string title);
            void setPosition(unsigned int x, unsigned int y);
            bool isOpen();
            bool pollEvent(IEvent& event);
            void close();
            void setVisible (bool visible);
            SDLWindow window;
        };
    }
}
#endif // ODFAEG_SDLWINDOWIMPL
