
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
            vertexArrayId = 0;
        }

        ////////////////////////////////////////////////////////////
        RenderWindow::RenderWindow(VideoMode mode, const String& title, Uint32 style, const ContextSettings& settings, bool useDepthDepth)
        {
            // Don't call the base class constructor because it contains virtual function calls
            this->useDepthTest = useDepthDepth;
            create(mode, title, style, settings);
            RenderTarget::setMajorVersion(settings.majorVersion);
            RenderTarget::setMinorVersion(settings.minorVersion);
            const GLubyte* glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);
            if(glslversion) {
                Shader::setVersionMajor(glslversion[0] - '0');
                Shader::setVersionMinor(glslversion[1] - '0');
            } else {
                Shader::setVersionMajor(1);
                Shader::setVersionMinor(3);
            }
            vertexArrayId = 0;
        }




        ////////////////////////////////////////////////////////////
        RenderWindow::RenderWindow(WindowHandle handle, const ContextSettings& settings)
        {
            // Don't call the base class constructor because it contains virtual function calls
            create(handle, settings);
            RenderTarget::setMajorVersion(settings.majorVersion);
            RenderTarget::setMinorVersion(settings.minorVersion);
            const GLubyte* glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);
            if(glslversion) {
                Shader::setVersionMajor(glslversion[0] - '0');
                Shader::setVersionMinor(glslversion[1] - '0');
            } else {
                Shader::setVersionMajor(2);
                Shader::setVersionMinor(0);
            }
            vertexArrayId = 0;
        }


        ////////////////////////////////////////////////////////////
        RenderWindow::~RenderWindow()
        {
            if (vertexArrayId != 0) {
                GLuint vao = reinterpret_cast<unsigned int>(vertexArrayId);
                glCheck(glDeleteVertexArrays(1, &vao));
            }
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
        Image RenderWindow::capture() const
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
        bool RenderWindow::isUsingDepthTest() const {
            return useDepthTest;
        }

        ////////////////////////////////////////////////////////////
        void RenderWindow::onCreate()
        {
            priv::ensureGlewInit();
            glewExperimental = GL_TRUE;
            GLenum status = glewInit();
            if (status == GLEW_OK)
            {
                std::cout<<"Glew initialized!"<<std::endl;
            }
            else
            {
                err() << "Failed to initialize GLEW, " << glewGetErrorString(status) << std::endl;
            }
            if (getSettings().majorVersion >= 3 && getSettings().minorVersion >= 3 && vertexArrayId == 0) {
                GLuint vao;
                glCheck(glGenVertexArrays(1, &vao));
                vertexArrayId = reinterpret_cast<unsigned int>(vao);
                glCheck(glBindVertexArray(vertexArrayId));
            }
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


