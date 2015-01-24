#include "../../../include/odfaeg/Graphics/sdlWindow.hpp"
namespace odfaeg {
    namespace graphic {
        SDLWindow::SDLWindow(unsigned int width, unsigned int height, std::string title) {
            window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED,
                                      SDL_WINDOWPOS_UNDEFINED,
                                      width,
                                      height,
                                      SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
        }
        void SDLWindow::setPosition(unsigned int x, unsigned int y) {
            SDL_SetWindowPosition(window, x, y);
        }
        bool SDLWindow::isOpen() {
            return window != nullptr;
        }
        void SDLWindow::close() {
            SDL_DestroyWindow(window);
            window = nullptr;
        }
        SDLWindow::~SDLWindow() {
            SDL_DestroyWindow(window);
        }
    }
}
