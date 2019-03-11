#include "../../../include/odfaeg/Window/window.hpp"
#include <SFML/Window/WindowStyle.hpp>
namespace
{
    const odfaeg::window::Window* fullscreenWindow = nullptr;
}
using namespace sf;
namespace odfaeg {
    namespace window {
        Window::Window() :
        m_window          (NULL),
        m_frameTimeLimit(sf::Time::Zero),
        m_size          (0, 0)
        {

        }
        ////////////////////////////////////////////////////////////
        Window::Window(VideoMode mode, const String& title, Uint32 style, const ContextSettings& settings) :
        m_window         (NULL),
        m_frameTimeLimit(sf::Time::Zero),
        m_size          (0, 0)
        {
            create(mode, title, style, settings);
        }
        ////////////////////////////////////////////////////////////
        Window::Window(WindowHandle handle, const ContextSettings& settings) :
        m_window          (NULL),
        m_frameTimeLimit(sf::Time::Zero),
        m_size          (0, 0)
        {
            create(handle, settings);
        }////////////////////////////////////////////////////////////
        void Window::create(VideoMode mode, const String& title, Uint32 style, const ContextSettings& settings)
        {
            m_window = WindowFactory::create();
            //SFML already have its own fullscreen window so we don't need to manage it.
            #if defined (SFML)
                m_window->create(mode, title, style, settings);
            #else
                // Destroy the previous window implementation
                m_window->destroy();

                // Fullscreen style requires some tests
                if (style & Style::Fullscreen)
                {
                    // Make sure there's not already a fullscreen window (only one is allowed)
                    if (fullscreenWindow)
                    {
                        std::cerr << "Creating two fullscreen windows is not allowed, switching to windowed mode" << std::endl;
                        style &= ~Style::Fullscreen;
                    }
                    else
                    {
                        // Make sure that the chosen video mode is compatible
                        if (!mode.isValid())
                        {
                            std::cerr << "The requested video mode is not available, switching to a valid mode" << std::endl;
                            mode = VideoMode::getFullscreenModes()[0];
                        }

                        // Update the fullscreen window
                        fullscreenWindow = this;
                    }
                }

                // Check validity of style according to the underlying platform
                #if defined(ODFAEG_SYSTEM_IOS) || defined(ODFAEG_SYSTEM_ANDROID)
                    if (style & Style::Fullscreen)
                        style &= ~Style::Titlebar;
                    else
                        style |= Style::Titlebar;
                #else
                    if ((style & Style::Close) || (style & Style::Resize))
                        style |= Style::Titlebar;
                #endif

                // Recreate the window implementation
                m_window->create(mode, title, style, settings);
            #endif
            // Perform common initializations
            initialize();
        }
        ////////////////////////////////////////////////////////////
        void Window::create(WindowHandle handle, const ContextSettings& settings)
        {
            m_window = WindowFactory::create();
            //SFML already destroy its own window so we don't need to manage it.
            #if defined (SFML)
                m_window->create(handle, settings);
            #else
                // Destroy the previous window implementation
                m_window->destroy();

                // Recreate the window implementation
                m_window->create(handle, settings);
                // Perform common initializations
            #endif
            initialize();
        }
        ////////////////////////////////////////////////////////////
        void Window::close()
        {
            //SFML already have its own fullscreen window so we don't need to maange it.
            #if defined (SFML)
                m_window->close();
            #else
                m_window->close();
                // Update the fullscreen window
                if (this == fullscreenWindow)
                    fullscreenWindow = NULL;
            #endif
        }
        ////////////////////////////////////////////////////////////
        bool Window::isOpen() const
        {
            return m_window != NULL && m_window->isOpen();
        }
        ////////////////////////////////////////////////////////////
        const ContextSettings& Window::getSettings() const
        {
            static const ContextSettings empty(0, 0, 0, 0, 0);

            return m_window ? m_window->getSettings() : empty;
        }
        ////////////////////////////////////////////////////////////
        bool Window::pollEvent(IEvent& event)
        {
            if (m_window && m_window->pollEvent(event))
            {
                return filterEvent(event);
            }
            else
            {
                return false;
            }
        }
        ////////////////////////////////////////////////////////////
        bool Window::waitEvent(IEvent& event)
        {
            if (m_window && m_window->waitEvent(event))
            {
                return filterEvent(event);
            }
            else
            {
                return false;
            }
        }
        ////////////////////////////////////////////////////////////
        Vector2i Window::getPosition() const
        {
            return m_window ? m_window->getPosition() : Vector2i();
        }
        ////////////////////////////////////////////////////////////
        void Window::setPosition(const Vector2i& position)
        {
            if (m_window)
                m_window->setPosition(position);
        }
        ////////////////////////////////////////////////////////////
        Vector2u Window::getSize() const
        {
            return m_size;
        }


        ////////////////////////////////////////////////////////////
        void Window::setSize(const Vector2u& size)
        {
            if (m_window)
            {
                m_window->setSize(size);

                // Cache the new size
                m_size.x = size.x;
                m_size.y = size.y;

                // Notify the derived class
                onResize();
            }
        }


        ////////////////////////////////////////////////////////////
        void Window::setTitle(const String& title)
        {
            if (m_window)
                m_window->setTitle(title);
        }


        ////////////////////////////////////////////////////////////
        void Window::setIcon(unsigned int width, unsigned int height, const Uint8* pixels)
        {
            if (m_window)
                m_window->setIcon(width, height, pixels);
        }


        ////////////////////////////////////////////////////////////
        void Window::setVisible(bool visible)
        {
            if (m_window)
                m_window->setVisible(visible);
        }


        ////////////////////////////////////////////////////////////
        void Window::setVerticalSyncEnabled(bool enabled)
        {
            if (m_window->setActive())
                m_window->setVerticalSyncEnabled(enabled);
        }


        ////////////////////////////////////////////////////////////
        void Window::setMouseCursorVisible(bool visible)
        {
            if (m_window)
                m_window->setMouseCursorVisible(visible);
        }


        ////////////////////////////////////////////////////////////
        void Window::setMouseCursorGrabbed(bool grabbed)
        {
            if (m_window)
                m_window->setMouseCursorGrabbed(grabbed);
        }


        ////////////////////////////////////////////////////////////
        void Window::setMouseCursor(const Cursor& cursor)
        {
            if (m_window)
                m_window->setMouseCursor(cursor);
        }


        ////////////////////////////////////////////////////////////
        void Window::setKeyRepeatEnabled(bool enabled)
        {
            if (m_window)
                m_window->setKeyRepeatEnabled(enabled);
        }


        ////////////////////////////////////////////////////////////
        void Window::setFramerateLimit(unsigned int limit)
        {
            //SFMl already handle it so just calling the SFML function.
            #if defined (SFML)
                m_window->setFrametimeLimit(limit)
            #else
                if (limit > 0)
                    m_frameTimeLimit = seconds(1.f / limit);
                else
                    m_frameTimeLimit = sf::Time::Zero;
            #endif
        }


        ////////////////////////////////////////////////////////////
        void Window::setJoystickThreshold(float threshold)
        {
            if (m_window)
                m_window->setJoystickThreshold(threshold);
        }


        ////////////////////////////////////////////////////////////
        bool Window::setActive(bool active)
        {
            if (m_window)
            {
                if (m_window->setActive(active))
                {
                    return true;
                }
                else
                {
                    std::cerr << "Failed to activate the window's context" << std::endl;
                    return false;
                }
            }
            else
            {
                return false;
            }
        }


        ////////////////////////////////////////////////////////////
        void Window::requestFocus()
        {
            if (m_window)
                m_window->requestFocus();
        }


        ////////////////////////////////////////////////////////////
        bool Window::hasFocus() const
        {
            return m_window && m_window->hasFocus();
        }


        ////////////////////////////////////////////////////////////

        void Window::display()
        {
            //SFMl already handle it so just calling the SFML function.
            #if defined(SFML)
                // Display the backbuffer on screen
                if (m_window.setActive())
                    m_window->display();
            #else
                 // Display the backbuffer on screen
                if (m_window->setActive())
                    m_window->display();
                // Limit the framerate if needed
                if (m_frameTimeLimit != Time::Zero)
                {
                    sleep(m_frameTimeLimit - m_clock.getElapsedTime());
                    m_clock.restart();
                }
            #endif
        }


        ////////////////////////////////////////////////////////////
        WindowHandle Window::getSystemHandle() const
        {
            return m_window ? m_window->getSystemHandle() : 0;
        }


        ////////////////////////////////////////////////////////////
        void Window::onCreate()
        {
            // Nothing by default
        }


        ////////////////////////////////////////////////////////////
        void Window::onResize()
        {
            // Nothing by default
        }


        ////////////////////////////////////////////////////////////
        bool Window::filterEvent(const IEvent& event)
        {
            // Notify resize events to the derived class
            if (event.type == IEvent::WINDOW_EVENT && event.window.type == IEvent::WINDOW_EVENT_RESIZED)
            {
                // Cache the new size
                m_size.x = event.window.data1;
                m_size.y = event.window.data2;

                // Notify the derived class
                onResize();
            }

            return true;
        }


        ////////////////////////////////////////////////////////////
        void Window::initialize()
        {
            //SFML already initialize its own window so we don't need to do it again.
            #if defined (SFML)
                // Get and cache the initial size of the window
                m_size = m_window->getSize();
            #else
                // Setup default behaviors (to get a consistent behavior across different implementations)
                setVisible(true);
                setMouseCursorVisible(true);
                setVerticalSyncEnabled(false);
                setKeyRepeatEnabled(true);
                setFramerateLimit(0);

                // Get and cache the initial size of the window
                m_size = m_window->getSize();

                // Reset frame time
                m_clock.restart();

                // Activate the window
                setActive();
            #endif
            // Notify the derived class
            onCreate();
        }
        IWindow* Window::getImpl() const {
            return m_window;
        }
        Window::~Window()
        {
            close();
        }
    }
}
