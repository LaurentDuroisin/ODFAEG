#include "../../../include/odfaeg/Graphics/sdlRenderWindow.hpp"
namespace odfaeg {
    namespace graphic {

        SDLRenderWindow::SDLRenderWindow(unsigned int width, unsigned int height, const std::string& title, const sf::ContextSettings& settings, bool useDepthTest) :
        SDLWindow(width, height, title)
        {
            context = SDL_GL_CreateContext(window);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, settings.majorVersion);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, settings.minorVersion);
            SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, settings.stencilBits);
            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, settings.depthBits);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, (settings.antialiasingLevel > 0) ? 1 : 0);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, settings.antialiasingLevel);
            size = sf::Vector2u(width, height);
            this->useDepthTest = useDepthTest;
        }
        bool SDLRenderWindow::activate(bool active) {
            if (active) {
                context = SDL_GL_CreateContext(window);
            } else {
                SDL_GL_DeleteContext(context);
            }
            return true;
        }
        sf::Vector2u SDLRenderWindow::getSize() const {
            return size;
        }
        bool SDLRenderWindow::isUsingDepthTest() const {
            return useDepthTest;
        }
        void SDLRenderWindow::display() {
            SDL_GL_SwapWindow(window);
        }
        SDLRenderWindow::~SDLRenderWindow() {
            SDL_GL_DeleteContext(context);
        }
    }
}
