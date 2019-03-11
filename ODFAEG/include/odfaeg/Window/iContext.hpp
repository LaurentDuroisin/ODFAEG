#ifndef ODFAEG_ICONTEXT_HPP
#define ODFAEG_ICONTEXT_HPP
#include "contextSettings.hpp"
#include <SFML/Window/WindowHandle.hpp>
namespace odfaeg {
    namespace window {
        typedef void (*GlFunctionPointer)();
        class IContext {
            public :
            virtual bool setActive(bool active) = 0;
            virtual void create(sf::WindowHandle handle, IContext* sharedContext = nullptr) = 0;
            virtual void create(ContextSettings& settings, unsigned int width, unsigned int height, IContext* sharedContext = nullptr) = 0;
            virtual const ContextSettings& getSettings() const = 0;
            virtual void display() = 0;
            virtual void setVerticalSyncEnabled(bool enabled) = 0;
        };
    }
}
#endif
