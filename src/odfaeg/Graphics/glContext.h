#ifndef ODFAEG_GLCONTEXT_HPP
#define ODFAEG_GLCONTEXT_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Config.hpp>
#include <SFML/Window/ContextSettings.hpp>
#include <SFML/System/NonCopyable.hpp>
#include <SDL2/SDL.h>
namespace odfaeg
{
namespace priv
{
////////////////////////////////////////////////////////////
/// \brief Abstract class representing an OpenGL context
///
////////////////////////////////////////////////////////////
class GlContext : sf::NonCopyable
{
public :

    ////////////////////////////////////////////////////////////
    /// \brief Perform the global initialization
    ///
    /// This function is called once, before the very first OpenGL
    /// resource is created. It makes sure that everything is ready
    /// for contexts to work properly.
    /// Note: this function doesn't need to be thread-safe, as it
    /// can be called only once.
    ///
    ////////////////////////////////////////////////////////////
    static void globalInit();

    ////////////////////////////////////////////////////////////
    /// \brief Perform the global cleanup
    ///
    /// This function is called after the very last OpenGL resource
    /// is destroyed. It makes sure that everything that was
    /// created by initialize() is properly released.
    /// Note: this function doesn't need to be thread-safe, as it
    /// can be called only once.
    ///
    ////////////////////////////////////////////////////////////
    static void globalCleanup();

    ////////////////////////////////////////////////////////////
    /// \brief Ensures that an OpenGL context is active in the current thread
    ///
    ////////////////////////////////////////////////////////////
    static void ensureContext();

    ////////////////////////////////////////////////////////////
    /// \brief Create a new context, not associated to a window
    ///
    /// This function automatically chooses the specialized class
    /// to use according to the OS.
    ///
    /// \return Pointer to the created context (don't forget to delete it)
    ///
    ////////////////////////////////////////////////////////////
    static GlContext* create();

    ////////////////////////////////////////////////////////////
    /// \brief Create a new context attached to a window
    ///
    /// This function automatically chooses the specialized class
    /// to use according to the OS.
    ///
    /// \param settings     Creation parameters
    /// \param owner        Pointer to the owner window
    /// \param bitsPerPixel Pixel depth (in bits per pixel)
    ///
    /// \return Pointer to the created context
    ///
    ////////////////////////////////////////////////////////////
    static GlContext* create(const sf::ContextSettings& settings, SDL_Window* window, unsigned int bitsPerPixel);

    ////////////////////////////////////////////////////////////
    /// \brief Create a new context that embeds its own rendering target
    ///
    /// This function automatically chooses the specialized class
    /// to use according to the OS.
    ///
    /// \param settings Creation parameters
    /// \param width    Back buffer width
    /// \param height   Back buffer height
    ///
    /// \return Pointer to the created context
    ///
    ////////////////////////////////////////////////////////////
    virtual void setVerticalSyncEnabled(bool enabled) = 0;
    virtual bool makeCurrent() = 0;
    virtual void display() = 0;
public :

    ////////////////////////////////////////////////////////////
    /// \brief Destructor
    ///
    ////////////////////////////////////////////////////////////
    virtual ~GlContext();

    ////////////////////////////////////////////////////////////
    /// \brief Get the settings of the context
    ///
    /// Note that these settings may be different than the ones
    /// passed to the constructor; they are indeed adjusted if the
    /// original settings are not directly supported by the system.
    ///
    /// \return Structure containing the settings
    ///
    ////////////////////////////////////////////////////////////
    const sf::ContextSettings& getSettings() const;

    ////////////////////////////////////////////////////////////
    /// \brief Activate or deactivate the context as the current target for rendering
    ///
    /// A context is active only on the current thread, if you want to
    /// make it active on another thread you have to deactivate it
    /// on the previous thread first if it was active.
    /// Only one context can be active on a thread at a time, thus
    /// the context previously active (if any) automatically gets deactivated.
    ///
    /// \param active True to activate, false to deactivate
    ///
    /// \return True if operation was successful, false otherwise
    ///
    ////////////////////////////////////////////////////////////
    bool setActive(bool active);

protected :

    ////////////////////////////////////////////////////////////
    /// \brief Default constructor
    ///
    /// This constructor is meant for derived classes only.
    ///
    ////////////////////////////////////////////////////////////
    GlContext();

    ////////////////////////////////////////////////////////////
    /// \brief Evaluate a pixel format configuration
    ///
    /// This functions can be used by implementations that have
    /// several valid formats and want to get the best one.
    /// A score is returned for the given configuration: the
    /// lower the score is, the better the configuration is.
    ///
    /// \param bitsPerPixel Requested pixel depth (bits per pixel)
    /// \param settings     Requested additionnal settings
    /// \param colorBits    Color bits of the configuration to evaluate
    /// \param depthBits    Depth bits of the configuration to evaluate
    /// \param stencilBits  Stencil bits of the configuration to evaluate
    /// \param antialiasing Antialiasing level of the configuration to evaluate
    ///
    /// \return Score of the configuration
    ///
    ////////////////////////////////////////////////////////////
    static int evaluateFormat(unsigned int bitsPerPixel, const sf::ContextSettings& settings, int colorBits, int depthBits, int stencilBits, int antialiasing);

    ////////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
    sf::ContextSettings m_settings; ///< Creation settings of the context

private:

    ////////////////////////////////////////////////////////////
    /// \brief Perform various initializations after the context construction
    ///
    ////////////////////////////////////////////////////////////
    void initialize();
};

} // namespace priv

} // namespace sf


#endif // SFML_GLCONTEXT_HPP
