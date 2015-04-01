#include "../../../include/odfaeg/Window/sdlWindow.hpp"
#include <iostream>
namespace odfaeg {
    namespace window {
        SDLWindow::SDLWindow() {
            window = nullptr;
        }
        SDLWindow::SDLWindow(unsigned int width, unsigned int height, std::string title) {
            window = nullptr;
            create(width, height, title);
        }
        void SDLWindow::create(unsigned int width, unsigned int height, std::string title) {
            window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED,
                                      SDL_WINDOWPOS_UNDEFINED,
                                      width,
                                      height,
                                      SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
            initialize();
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
        bool SDLWindow::pollEvent(SDL_Event& event) {
            return SDL_PollEvent(&event);
        }
        void SDLWindow::initialize() {
            onCreate();
        }
        void SDLWindow::onCreate() {
        }
        void SDLWindow::setVisible (bool visible) {
            if (!visible)
                SDL_HideWindow(window);
            else
                SDL_ShowWindow(window);
        }
        SDLWindow::~SDLWindow() {
            if (window != nullptr)
                SDL_DestroyWindow(window);
        }
    }
}
