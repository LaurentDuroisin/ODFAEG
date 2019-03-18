#include "../../../include/odfaeg/Window/windowImpl.hpp"
using namespace sf;
namespace odfaeg {
    namespace window {
        WindowImpl::WindowImpl()
        {
        }
        ////////////////////////////////////////////////////////////
        void WindowImpl::create(VideoMode mode, const String& title, Uint32 style, const ContextSettings& settings)
        {
            std::cout<<"create odfaeg window"<<std::endl;
            // Recreate the WindowImpl implementation
            WindowImplType::create(mode, title, style, settings);
            m_context.create(getSystemHandle(), settings);
        }
        ////////////////////////////////////////////////////////////
        void WindowImpl::create(WindowHandle handle, const ContextSettings& settings)
        {
            // Recreate the WindowImpl implementation
            WindowImplType::create(handle, settings);
            m_context.create(handle, settings);
        }
        ////////////////////////////////////////////////////////////
        void WindowImpl::close()
        {
            WindowImplType::close();
        }
        ////////////////////////////////////////////////////////////
        bool WindowImpl::isOpen() const
        {
            return WindowImplType::isOpen();
        }
        ////////////////////////////////////////////////////////////
        bool WindowImpl::pollEvent(IEvent& event)
        {
            return popEvent(event);
        }
        ////////////////////////////////////////////////////////////
        bool WindowImpl::waitEvent(IEvent& event) {
            //For later.
        }
        ////////////////////////////////////////////////////////////
        Vector2i WindowImpl::getPosition() const
        {
            return WindowImplType::getPosition();
        }


        ////////////////////////////////////////////////////////////
        void WindowImpl::setPosition(const Vector2i& position)
        {
            WindowImplType::setPosition(position);
        }
        ////////////////////////////////////////////////////////////
        Vector2u WindowImpl::getSize() const
        {
            return WindowImplType::getSize();
        }
        ////////////////////////////////////////////////////////////
        void WindowImpl::setSize(const Vector2u& size)
        {
            WindowImplType::setSize(size);
        }
        ////////////////////////////////////////////////////////////
        void WindowImpl::setTitle(const String& title)
        {
            WindowImplType::setTitle(title);
        }
        ////////////////////////////////////////////////////////////
        void WindowImpl::setIcon(unsigned int width, unsigned int height, const Uint8* pixels)
        {
            WindowImplType::setIcon(width, height, pixels);
        }
        ////////////////////////////////////////////////////////////
        void WindowImpl::setVisible(bool visible)
        {
            WindowImplType::setVisible(visible);
        }
        ////////////////////////////////////////////////////////////
        void WindowImpl::setMouseCursorVisible(bool visible)
        {
            WindowImplType::setMouseCursorVisible(visible);
        }


        ////////////////////////////////////////////////////////////
        void WindowImpl::setMouseCursorGrabbed(bool grabbed)
        {
            WindowImplType::setMouseCursorGrabbed(grabbed);
        }


        ////////////////////////////////////////////////////////////
        void WindowImpl::setMouseCursor(const sf::Cursor& cursor)
        {
            //For later.
        }
        ////////////////////////////////////////////////////////////
        void WindowImpl::setKeyRepeatEnabled(bool enabled)
        {
           WindowImplType::setKeyRepeatEnabled(enabled);
        }
        ////////////////////////////////////////////////////////////
        void WindowImpl::setJoystickThreshold(float threshold)
        {
            //For later.
        }
        ////////////////////////////////////////////////////////////
        void WindowImpl::requestFocus()
        {
            WindowImplType::requestFocus();
        }


        ////////////////////////////////////////////////////////////
        bool WindowImpl::hasFocus() const
        {
            WindowImplType::hasFocus();
        }
        ////////////////////////////////////////////////////////////
        WindowHandle WindowImpl::getSystemHandle() const
        {
            return WindowImplType::getSystemHandle();
        }
        void WindowImpl::destroy() {
            WindowImplType::destroy();
        }
        bool WindowImpl::setActive(bool active) {
            return m_context.setActive(active);
        }
        void WindowImpl::setVerticalSyncEnabled(bool enabled) {
            m_context.setVerticalSyncEnabled(enabled);
        }
        void WindowImpl::display() {
            m_context.display();
        }
        void WindowImpl::setFramerateLimit(unsigned int limit) {
            //Nothing to do because I do it in the Window class.
        }
        void WindowImpl::initialize() {
        }
        const ContextSettings& WindowImpl::getSettings() const {
            return m_context.getSettings();
        }
        ////////////////////////////////////////////////////////////
        WindowImpl::~WindowImpl()
        {
            close();
        }
    }
}
