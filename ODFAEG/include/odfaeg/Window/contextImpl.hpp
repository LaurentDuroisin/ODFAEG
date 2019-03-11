#ifndef ODFAEG_CONTEXTIMPL_HPP
#define ODFAEG_CONTEXTIMPL_HPP
#include <SFML/System/ThreadLocalPtr.hpp>
#include "contextSettings.hpp"
#include "../../../include/odfaeg/config.hpp"
#if defined(ODFAEG_SYSTEM_LINUX)
#include "../../../include/odfaeg/Window/glxContext.hpp"
typedef odfaeg::window::GlxContext ContextImplType;
#endif
#include <vector>
#include <string>
namespace odfaeg {
    namespace window {
        class ContextImpl : public ContextImplType {
            public :
                ContextImpl();
                static void initResource();
                static void cleanupResource();
                static GlFunctionPointer getFunction(const char* name);
                static int evaluateFormat(unsigned int bitsPerPixel, const ContextSettings& settings, int colorBits, int depthBits, int stencilBits, int antialiasing, bool accelerated, bool sRgb);
                static bool isExtensionAvalaible(const char* name);
                void create(sf::WindowHandle handle, IContext* shared = nullptr);
                void create(ContextSettings& settings, unsigned int width, unsigned int height, IContext* shared = nullptr);
                bool setActive(bool active);
                const ContextSettings& getSettings() const;
                void display();
                void setVerticalSyncEnabled(bool enabled);
                ~ContextImpl();
            private :
                static std::vector<std::string> extensions;
                static unsigned int nbContexts;
                static IContext* sharedContext;
        };
    }
}
#endif
