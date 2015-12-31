#ifndef SDL_WINDOW
#define SDL_WINDOW
#include <SDL2/SDL.h>
namespace odfaeg {
    namespace graphic {
        class SDL_Window {
            public :
            SDL_Window(unsigned int width, unsigned int height, std::string title);
            void setPosition(unsigned int x, unsigned int y);
            bool pollEvent(SDL_Event* event);
            ~Window();
            protected :
                SDL_Window* window;
            private :
            bool SDL_RenderWindow::sdl_initialized = false;
        };
    }
}
#endif // SDL_WINDOW
