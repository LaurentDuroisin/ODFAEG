#ifndef ODFAEG_GLX_CONTEXT
#define ODFAEG_GLX_CONTEXT
#include "contextSettings.hpp"
#include "iContext.hpp"
#include <GL/glx.h>
namespace odfaeg {
    namespace window {
        class GlxContext : public IContext {
            public :
                GlxContext();
                static XVisualInfo selectBestVisual(::Display* display, unsigned int bitsPerPixel, const ContextSettings& settings);
                virtual bool setActive(bool current);
                virtual void create(::Window window, IContext* shared = nullptr);
                virtual void create(ContextSettings& settings, unsigned int width, unsigned int height, IContext* shared = nullptr);
                virtual const ContextSettings& getSettings() const;
                virtual void display();
                void setVerticalSyncEnabled(bool enabled);
                ~GlxContext();
                bool m_windowLess;
            private :
                void checkSettings ();
                static bool isExtensionSupported(const char *extList, const char *extension);
                static int ctxErrorHandler(_XDisplay *dpy, XErrorEvent *ev );
                static bool ctxErrorOccurred;
                ::Display* m_display;
                ::Window m_window;
                ContextSettings settings;
                GLXContext ctx;
                GLXPbuffer pbuf;
        };
    }
}
#endif
