#include "../../../include/odfaeg/Driver/context.hpp"
namespace odfaeg {
    namespace driver {
        unsigned int Context::nbContexts = 0;
        Context (Window& window) : frameBuffer(window.getSize().x, window.getSize().y) {
            contextId = nbContexts;
            nbContexts++;
        }
        Context::Context(unsigned int width, unsigned int height) : frameBuffer(width, height) {
            contextId = nbContexts;
            nbContexts++;
        }
        void Context::addCommand(FastDelegate<void> command) {
            commands.push_back(command);
        }
        void Context::addResource(core::any resource) {
            resources.push_back(resource);
        }
        T Context::getResource(unsigned int id) {
            if (id >= 0 && id < resource.size()) {
                return resources[id].get<T>();
            throw core::Error(60, "context : resource index out of range", 0);
        }
        unsigned int Context::getId() {
            return contextId;
        }
    }
}
