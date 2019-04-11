#ifndef ODFAEG_CONTEXT_HPP
#define ODFAEG_CONTEXT_HPP
#include "command.hpp"
#include "frameBuffer.hpp"
namespace odfaeg {
    namespace driver {
        class DriverContext {
            public :
                void Context(window::Window& window);
                void Context(unsigned int width, unsigned int height);
                void addCommand(FastDelegate<void> command);
                static void addResource(core::any resource);
                template<typename T>
                static T getResource();
                unsigned int getId();
            private :
                DriverCommand commands;
                static std::vector<core::any> resources;
                unsigned int contextId;
                static unsigned int nbContexts;
                FrameBuffer frameBuffer;
        };
    }
}
#endif
