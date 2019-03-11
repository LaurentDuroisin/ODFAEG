#include "../../../include/odfaeg/Window/contextImpl.hpp"
#include <SFML/System/Mutex.hpp>
#include <SFML/System/Lock.hpp>
#include <iostream>
#if defined(ODFAEG_SYSTEM_WINDOWS)

    typedef const GLubyte* (APIENTRY *glGetStringiFuncType)(GLenum, GLuint);

#else

    typedef const GLubyte* (*glGetStringiFuncType)(GLenum, GLuint);

#endif
using namespace sf;
namespace {
    ThreadLocalPtr<odfaeg::window::ContextImpl> current_ContextImpl (nullptr);
    sf::Mutex mutex;
}
namespace odfaeg {
    namespace window {
        using namespace sf;
        IContext* ContextImpl::sharedContext = nullptr;
        std::vector<std::string> ContextImpl::extensions = std::vector<std::string>();
        unsigned int ContextImpl::nbContexts = 0;
        void ContextImpl::initResource() {
            if (nbContexts == 0) {
                sharedContext = new ContextImplType();
                ContextSettings settings;
                sharedContext->create(settings, 1, 1);
                sharedContext->setActive(true);
                nbContexts++;
            }
            sharedContext->setActive(false);
        }
        void ContextImpl::cleanupResource() {
            nbContexts--;
            if (nbContexts == 0) {
                sharedContext->setActive(false);
                delete sharedContext;
            }
        }
        ContextImpl::ContextImpl() {
            if (nbContexts == 0) {
                sharedContext = new ContextImplType();
                ContextSettings settings;
                sharedContext->create(settings, 1, 1);
                sharedContext->setActive(true);
                // Load our extensions vector
                extensions.clear();

                // Check whether a >= 3.0 context is available
                int majorVersion = 0;
                glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);

                if (glGetError() == GL_INVALID_ENUM)
                {
                    // Try to load the < 3.0 way
                    const char* extensionString = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));

                    do
                    {
                        const char* extension = extensionString;

                        while(*extensionString && (*extensionString != ' '))
                            extensionString++;

                        extensions.push_back(std::string(extension, extensionString));
                    }
                    while (*extensionString++);
                }
                else
                {
                    // Try to load the >= 3.0 way
                    glGetStringiFuncType glGetStringiFunc = NULL;
                    glGetStringiFunc = reinterpret_cast<glGetStringiFuncType>(getFunction("glGetStringi"));

                    if (glGetStringiFunc)
                    {
                        int numExtensions = 0;
                        glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

                        if (numExtensions)
                        {
                            for (unsigned int i = 0; i < static_cast<unsigned int>(numExtensions); ++i)
                            {
                                const char* extensionString = reinterpret_cast<const char*>(glGetStringiFunc(GL_EXTENSIONS, i));

                                extensions.push_back(extensionString);
                            }
                        }
                    }
                }
                nbContexts++;
            }
            sharedContext->setActive(false);
        }
        void ContextImpl::create(ContextSettings& settings, unsigned int width, unsigned int height, IContext* shared) {
            ContextImplType::create(settings, width, height, sharedContext);
        }
        void ContextImpl::create(sf::WindowHandle handle, IContext* shared) {
            ContextImplType::create(handle, sharedContext);
        }
        bool ContextImpl::setActive(bool active) {
            if (active) {
                if (this != current_ContextImpl) {
                    Lock lock(mutex);
                    if (ContextImplType::setActive(true)) {
                        current_ContextImpl = this;

                        return true;
                    } else {
                        return false;
                    }
                } else {
                    return true;
                }
            } else {
                if (this == current_ContextImpl) {
                    if (ContextImplType::setActive(false)) {
                        current_ContextImpl = nullptr;
                        return true;
                    } else {
                        return false;
                    }
                } else {
                    return true;
                }
            }
        }
        const ContextSettings& ContextImpl::getSettings() const {
            return ContextImplType::getSettings();
        }
        void ContextImpl::display() {
            ContextImplType::display();
        }
        void ContextImpl::setVerticalSyncEnabled (bool enabled) {
            //For later;
        }
        ////////////////////////////////////////////////////////////
        int ContextImpl::evaluateFormat(unsigned int bitsPerPixel, const ContextSettings& settings, int colorBits, int depthBits, int stencilBits, int antialiasing, bool accelerated, bool sRgb)
        {
            int colorDiff        = static_cast<int>(bitsPerPixel)               - colorBits;
            int depthDiff        = static_cast<int>(settings.depthBits)         - depthBits;
            int stencilDiff      = static_cast<int>(settings.stencilBits)       - stencilBits;
            int antialiasingDiff = static_cast<int>(settings.antiAliasingLevel) - antialiasing;

            // Weight sub-scores so that better settings don't score equally as bad as worse settings
            colorDiff        *= ((colorDiff        > 0) ? 100000 : 1);
            depthDiff        *= ((depthDiff        > 0) ? 100000 : 1);
            stencilDiff      *= ((stencilDiff      > 0) ? 100000 : 1);
            antialiasingDiff *= ((antialiasingDiff > 0) ? 100000 : 1);

            // Aggregate the scores
            int score = std::abs(colorDiff) + std::abs(depthDiff) + std::abs(stencilDiff) + std::abs(antialiasingDiff);

            // If the user wants an sRGB capable format, try really hard to get one
            if (settings.sRgbCapable && !sRgb)
                score += 10000000;

            // Make sure we prefer hardware acceleration over features
            if (!accelerated)
                score += 100000000;

            return score;
        }
        ////////////////////////////////////////////////////////////
        GlFunctionPointer ContextImpl::getFunction(const char* name)
        {
            #if !defined(ODFAEG_OPENGL_ES)

                Lock lock(mutex);

                return ContextImplType::getFunction(name);

            #else

                return 0;

            #endif
        }
        bool ContextImpl::isExtensionAvalaible(const char* name) {
            for (unsigned int i = 0; i < extensions.size(); i++) {
                if (extensions[i] == name)
                    return true;
            }
            return false;
        }
        ContextImpl::~ContextImpl() {
            nbContexts--;
            if (nbContexts == 0) {
                sharedContext->setActive(false);
                delete sharedContext;
            }
        }
    }
}
