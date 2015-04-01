#ifndef SDL_WINDOW
#define SDL_WINDOW
#include <SDL2/SDL.h>
#include <string>
namespace odfaeg {
    namespace window {
        class SDLWindow {
            public :
            SDLWindow();
            SDLWindow(unsigned int width, unsigned int height, std::string title);
            void create(unsigned int width, unsigned int height, std::string title);
            void setPosition(unsigned int x, unsigned int y);
            virtual void onCreate();
            bool isOpen();
            void close();
            bool pollEvent(SDL_Event &event);
            void setVisible (bool visible);
            virtual ~SDLWindow();
            public :
            void initialize();
            SDL_Window* window;
        };
    }
}
#endif // SDL_WINDOW
