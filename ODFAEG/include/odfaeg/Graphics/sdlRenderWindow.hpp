#ifndef SDL_RENDER_WINDOW_HPP
#define SDL_RENDER_WINDOW_HPP
#include "renderTarget.h"
#include "../../../include/odfaeg/Window/sdlWindow.hpp"
#include "../../../include/odfaeg/Window/contextSettings.hpp"
#include <SFML/Window.hpp>
#include <SDL2/SDL.h>
namespace odfaeg {
    namespace graphic {
        class SDLRenderWindow : public window::SDLWindow, public RenderTarget {
            public:
            SDLRenderWindow(unsigned int width, unsigned int height, const std::string& title, const window::ContextSettings& settings = window::ContextSettings(), bool useDepthTest = false);
            void display();
            sf::Vector2u getSize() const;
            bool isUsingDepthTest() const;
            bool activate(bool active);
            bool isActive();
            void onCreate();
            ~SDLRenderWindow();
            private:
            bool activated;
            SDL_GLContext context;
            bool useDepthTest;
            sf::Vector2u size;
            window::ContextSettings settings;
            unsigned int vertexArrayId;
        };
    }
}
#endif // SDL_RENDER_WINDOW_HPP
