#ifndef ODFAEG_SDL_GLCONTEXT_HPP
#define ODFAEG_SDL_GLCONTEXT_HPP
#include "glContext.h"
#include "../../../include/odfaeg/Graphics/videoMode.h"


namespace odfaeg
{
namespace priv
{
////////////////////////////////////////////////////////////
/// \brief Linux (GLX) implementation of OpenGL contexts
///
////////////////////////////////////////////////////////////
class SDL_GL_Context : public GlContext
{
public :

    ////////////////////////////////////////////////////////////
    /// \brief Create a new default context
    ///
    /// \param shared Context to share the new one with (can be NULL)
    ///
    ////////////////////////////////////////////////////////////
    SDL_GL_Context();

    ////////////////////////////////////////////////////////////
    /// \brief Create a new context attached to a window
    ///
    /// \param shared       Context to share the new one with
    /// \param settings     Creation parameters
    /// \param owner        Pointer to the owner window
    /// \param bitsPerPixel Pixel depth, in bits per pixel
    ///
    ////////////////////////////////////////////////////////////
    SDL_GL_Context(const sf::ContextSettings& settings, SDL_Window* window, unsigned int bitsPerPixel);
    void createContext(unsigned int bitsPerPixel, const sf::ContextSettings& settings);
    void setVerticalSyncEnabled(bool enabled);
    bool makeCurrent();
    void display();
    sf::ContextSettings getSettings();
    ////////////////////////////////////////////////////////////
    /// \brief Destructor
    ///
    ////////////////////////////////////////////////////////////
    ~SDL_GL_Context();
private :
    SDL_GLContext m_context;
    SDL_Window* m_window;
    bool m_ownWindow;
    sf::ContextSettings m_settings;
};

} // namespace priv

} // namespace sf

#endif // SFML_GLXCONTEXT_HPP

