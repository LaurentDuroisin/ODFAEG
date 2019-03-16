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
#include "../../../include/odfaeg/Window/context.hpp"
#include <SFML/System/Lock.hpp>
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
                void create(IContext* shared);
                void create(sf::WindowHandle handle, IContext* shared = nullptr);
                void create(ContextSettings& settings, unsigned int width, unsigned int height, IContext* shared = nullptr);
                bool setActive(bool active);
                const ContextSettings& getSettings() const;
                void display();
                void setVerticalSyncEnabled(bool enabled);
                static void acquireTransientContext();
                static void releaseTransientContext();
                ~ContextImpl();
            private :
                // OpenGL resources counter
                static unsigned int resourceCount;
                static std::vector<std::string> extensions;
                static unsigned int nbContexts;
                static IContext* sharedContext;
                static sf::ThreadLocalPtr<odfaeg::window::ContextImpl> current_ContextImpl;
                static sf::Mutex mutex;
                // This structure contains all the state necessary to
                // track TransientContext usage
                // This per-thread variable tracks if and how a transient
                // context is currently being used on the current thread
                struct TransientContext : private sf::NonCopyable
                {
                    ////////////////////////////////////////////////////////////
                    /// \brief Constructor
                    ///
                    ////////////////////////////////////////////////////////////
                    TransientContext() :
                    referenceCount   (0),
                    context          (0),
                    sharedContextLock(0),
                    useSharedContext (false)
                    {
                        if (resourceCount == 0)
                        {
                            context = new odfaeg::window::Context;
                        }
                        else if (!current_ContextImpl)
                        {
                            sharedContextLock = new sf::Lock(mutex);
                            useSharedContext = true;
                            sharedContext->setActive(true);
                        }
                    }

                    ////////////////////////////////////////////////////////////
                    /// \brief Destructor
                    ///
                    ////////////////////////////////////////////////////////////
                    ~TransientContext()
                    {
                        if (useSharedContext)
                            sharedContext->setActive(false);

                        delete sharedContextLock;
                        delete context;
                    }

                    ///////////////////////////////////////////////////////////
                    // Member data
                    ////////////////////////////////////////////////////////////
                    unsigned int referenceCount;
                    Context* context;
                    sf::Lock*    sharedContextLock;
                    bool         useSharedContext;
                };
                static sf::ThreadLocalPtr<TransientContext> transientContext;
        };
    }
}
#endif
