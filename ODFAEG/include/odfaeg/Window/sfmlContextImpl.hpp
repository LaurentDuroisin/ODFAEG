#ifndef ODFAEG_SFML_CONTEXT_IMPL_HPP
#define ODFAEG_SFML_CONTEXT_IMPL_HPP
#include <SFML/Window/Context.hpp>
#include "iContext.hpp"
namespace odfaeg {
    namespace window {
        class SFMLContextImpl : public IContext {
        public :
            bool setActive(bool active);
            void create(sf::WindowHandle handle, IContext* sharedContext = nullptr);
            void create(ContextSettings& settings, unsigned int width, unsigned int height, IContext* sharedContext = nullptr);
            const ContextSettings& getSettings() const;
            void display();
            void setVerticalSyncEnabled(bool enabled);
        private :
            sf::Context* context;
        };
    }
}
#endif
