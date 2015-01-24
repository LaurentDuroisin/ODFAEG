#include "sdlglContext.h"
#include <SFML/OpenGL.hpp>
#include <SFML/System/Err.hpp>
#include <iostream>
namespace odfaeg
{
namespace priv
{
////////////////////////////////////////////////////////////
SDL_GL_Context::SDL_GL_Context() :
m_window(nullptr),
m_ownWindow (true)
{
    /*m_window = SDL_CreateWindow("ODFAEG_API", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1, 1,SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
    createContext(32, sf::ContextSettings());*/
}
SDL_GL_Context::SDL_GL_Context(const sf::ContextSettings& settings, SDL_Window* window, unsigned int bitsPerPixel) :
m_window(window),
m_ownWindow (false) {
    if (m_window)
        createContext(bitsPerPixel, settings);
}
bool SDL_GL_Context::makeCurrent() {
    return m_context && m_window && SDL_GL_MakeCurrent(m_window, m_context);
}
////////////////////////////////////////////////////////////
SDL_GL_Context::~SDL_GL_Context()
{
    // Destroy the context
    if (m_context)
    {
        SDL_GL_DeleteContext(m_context);
    }
}

////////////////////////////////////////////////////////////
void SDL_GL_Context::display()
{
    if (m_window && m_context)
        SDL_GL_SwapWindow(m_window);
}


////////////////////////////////////////////////////////////
void SDL_GL_Context::setVerticalSyncEnabled(bool enabled)
{
    if (enabled)
        SDL_GL_SetSwapInterval(1);
    else
        SDL_GL_SetSwapInterval(0);
}

////////////////////////////////////////////////////////////
void SDL_GL_Context::createContext(unsigned int bitsPerPixel, const sf::ContextSettings& settings)
{
    // Save the creation settings
    m_settings = settings;

    // Create the OpenGL context -- first try context versions >= 3.0 if it is requested (they require special code)
    if (m_settings.majorVersion >= 3)
    {

        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, settings.depthBits);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, settings.stencilBits);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, settings.antialiasingLevel > 0);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, settings.antialiasingLevel);
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, bitsPerPixel == 32 ? 8 : 0);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, true);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, static_cast<int>(m_settings.majorVersion));
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, static_cast<int>(m_settings.minorVersion));
        m_context = SDL_GL_CreateContext(m_window);
        std::cout<<SDL_GetError()<<std::endl;

    }

    // If the OpenGL >= 3.0 context failed or if we don't want one, create a regular OpenGL 1.x/2.x context
    if (!&m_context)
    {
        // set the context version to 2.0 (arbitrary)
        m_settings.majorVersion = 2;
        m_settings.minorVersion = 0;
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, static_cast<int>(m_settings.majorVersion));
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, static_cast<int>(m_settings.minorVersion));

        // Create the context, using the target window's visual
        m_context = SDL_GL_CreateContext(m_window);
        std::cout<<"context : "<<&m_context<<std::endl;
        if (!&m_context)
        {
            sf::err() << "Failed to create an OpenGL context for this window" << std::endl;
            return;
        }
    }

    // Update the creation settings from the chosen format
    int depth, stencil, multiSampling, samples;
    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &depth);
    SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &stencil);
    SDL_GL_GetAttribute(SDL_GL_MULTISAMPLEBUFFERS, &samples);
    SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &multiSampling);
    m_settings.depthBits         = static_cast<unsigned int>(depth);
    m_settings.stencilBits       = static_cast<unsigned int>(stencil);
    m_settings.antialiasingLevel = multiSampling ? samples : 0;
}

} // namespace priv

} // namespace sf

