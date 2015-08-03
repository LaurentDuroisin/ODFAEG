#include "../../../include/odfaeg/Graphics/sdlRenderWindow.hpp"
#include "glCheck.h"
namespace odfaeg {
    namespace graphic {
        using namespace sf;
        SDLRenderWindow::SDLRenderWindow(unsigned int width, unsigned int height, const std::string& title, const sf::ContextSettings& settings, bool useDepthTest)
        {
            size = sf::Vector2u(width, height);
            this->useDepthTest = useDepthTest;
            this->settings = settings;
            activated = false;
            create(width, height, title);
        }
        bool SDLRenderWindow::activate(bool active) {
            if (active && !activated) {
                if(SDL_GL_MakeCurrent(window, context)) {
                    return false;
                } else {
                    activated = true;
                    return true;
                }
            } else if (!active && activated) {
                if(SDL_GL_MakeCurrent(window, nullptr)) {
                    return false;
                } else {
                    activated = false;
                    return true;
                }
            }
            return true;
        }
        bool SDLRenderWindow::isActive() {
            return activated;
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
        void SDLRenderWindow::onCreate() {
            SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, settings.majorVersion);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, settings.minorVersion);
            SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, settings.stencilBits);
            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, settings.depthBits);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, (settings.antialiasingLevel > 0) ? 1 : 0);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, settings.antialiasingLevel);
            context = SDL_GL_CreateContext(window);
            priv::ensureGlewInit();
            /*glewExperimental = GL_TRUE;
            GLenum status = glewInit();
            if (status == GLEW_OK)
            {
                std::cout<<"Glew initialized!"<<std::endl;
            }
            else
            {
                err() << "Failed to initialize GLEW, " << glewGetErrorString(status) << std::endl;
            }*/
            if (settings.majorVersion >= 3 && settings.minorVersion >= 3 && vertexArrayId == 0) {
                GLuint vao;
                glCheck(glGenVertexArrays(1, &vao));
                vertexArrayId = reinterpret_cast<unsigned int>(vao);
                glCheck(glBindVertexArray(vertexArrayId));
            }
            // Just initialize the render target part
            RenderTarget::initialize();
        }
        SDLRenderWindow::~SDLRenderWindow() {
            SDL_GL_DeleteContext(context);
        }
    }
}
