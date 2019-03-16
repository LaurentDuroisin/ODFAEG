#include <SFML/OpenGL.hpp>
#include "../../../include/odfaeg/Graphics/renderWindow.h"
#include "glCheck.h"
//#include "GlDebug.hpp"

namespace odfaeg {
    namespace graphic {
        using namespace sf;
        ////////////////////////////////////////////////////////////
        RenderWindow::RenderWindow()
        {
            // Nothing to do
        }

        ////////////////////////////////////////////////////////////
        RenderWindow::RenderWindow(VideoMode mode, const String& title, Uint32 style, const window::ContextSettings& settings)
        {
            // Don't call the base class constructor because it contains virtual function calls
            create(mode, title, style, settings);
        }




        ////////////////////////////////////////////////////////////
        RenderWindow::RenderWindow(WindowHandle handle, const window::ContextSettings& settings)
        {
            // Don't call the base class constructor because it contains virtual function calls
            create(handle, settings);
        }


        ////////////////////////////////////////////////////////////
        RenderWindow::~RenderWindow()
        {

        }


        ////////////////////////////////////////////////////////////
        bool RenderWindow::activate(bool active)
        {

            return setActive(active);
        }


        ////////////////////////////////////////////////////////////
        Vector2u RenderWindow::getSize() const
        {
            return Window::getSize();
        }

        ////////////////////////////////////////////////////////////
        Image RenderWindow::capture()
        {
            Image image;
            if (setActive())
            {
                int width = static_cast<int>(getSize().x);
                int height = static_cast<int>(getSize().y);

                // copy rows one by one and flip them (OpenGL's origin is bottom while SFML's origin is top)
                std::vector<Uint8> pixels(width * height * 4);
                for (int i = 0; i < height; ++i)
                {
                    Uint8* ptr = &pixels[i * width * 4];
                    glCheck(glReadPixels(0, height - i - 1, width, 1, GL_RGBA, GL_UNSIGNED_BYTE, ptr));
                }

                image.create(width, height, &pixels[0]);
            }

            return image;
        }
        ////////////////////////////////////////////////////////////
        void RenderWindow::onCreate()
        {
            priv::ensureGlewInit();
            RenderTarget::setVersionMajor(getSettings().versionMajor);
            RenderTarget::setVersionMinor(getSettings().versionMinor);
            // Just initialize the render target part
            RenderTarget::initialize();
        }


        ////////////////////////////////////////////////////////////
        void RenderWindow::onResize()
        {
            // Update the current view (recompute the viewport, which is stored in relative coordinates)
            setView(getView());
        }
    }

} // namespace sf


