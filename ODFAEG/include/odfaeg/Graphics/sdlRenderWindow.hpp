#ifndef SDL_RENDER_WINDOW_HPP
#define SDL_RENDER_WINDOW_HPP
#include "renderTarget.h"
#include "sdlWindow.hpp"
#include <SFML/Window.hpp>
#include <SDL2/SDL.h>
namespace odfaeg {
    namespace graphic {
        class SDLRenderWindow : public SDLWindow, public RenderTarget {
            public:
            SDLRenderWindow(unsigned int width, unsigned int height, const std::string& title, const sf::ContextSettings& settings = sf::ContextSettings(), bool useDepthTest = false);
            void display();
            sf::Vector2u getSize() const;
            bool isUsingDepthTest() const;
            bool activate(bool active);
            ~SDLRenderWindow();
            private:
            SDL_GLContext context;
            bool useDepthTest;
            sf::Vector2u size;
        };
    }
}
#endif // SDL_RENDER_WINDOW_HPP
