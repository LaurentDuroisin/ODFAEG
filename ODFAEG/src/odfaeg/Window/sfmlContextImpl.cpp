#include "../../../include/odfaeg/Window/sfmlContextImpl.hpp"
namespace odfaeg {
    namespace window {
        bool SFMLContextImpl::setActive(bool active) {
            if (context)
                return context->setActive(active);
            return false;
        }
        void SFMLContextImpl::create(sf::WindowHandle handle, IContext* sharedContext) {

        }
        void SFMLContextImpl::create(ContextSettings& settings, unsigned int width, unsigned int height, IContext* sharedContext) {
            context = new sf::Context(sf::ContextSettings(settings.depthBits, settings.stencilBits, settings.antiAliasingLevel, settings.versionMajor, settings.versionMinor), width, height);
        }
        const ContextSettings& SFMLContextImpl::getSettings() const {
            if (context) {
                sf::ContextSettings settings = context->getSettings();
                return ContextSettings(settings.depthBits, settings.stencilBits, settings.antialiasingLevel, settings.majorVersion, settings.minorVersion);
            } else {
                return ContextSettings(0, 0, 0, 0, 0);
            }
        }
        void SFMLContextImpl::display() {

        }
        void setVerticalSyncEnabled(bool enabled) {

        }
    }
}
