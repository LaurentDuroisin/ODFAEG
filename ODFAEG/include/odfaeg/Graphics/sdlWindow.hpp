#ifndef SDL_WINDOW
#define SDL_WINDOW
#include <SDL2/SDL.h>
#include <string>
namespace odfaeg {
    namespace graphic {
        class SDLWindow {
            public :
            SDLWindow(unsigned int width, unsigned int height, std::string title);
            void setPosition(unsigned int x, unsigned int y);
            virtual ~SDLWindow();
            bool isOpen();
            void close();
            protected :
                SDL_Window* window;
        };
    }
}
#endif // SDL_WINDOW
