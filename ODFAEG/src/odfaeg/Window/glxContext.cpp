#include "glxExtensions.hpp"
#include "../../../include/odfaeg/Window/glxContext.hpp"
#include "../../../include/odfaeg/Window/contextImpl.hpp"
#include "../../../include/odfaeg/Window/display.hpp"
#include <SFML/Window/VideoMode.hpp>
#include <cstring>
#include <iostream>
typedef GLXContext (*glXCreateContextAttribsARBProc)(::Display*, GLXFBConfig, GLXContext, Bool, const int*);
typedef Bool (*glXMakeContextCurrentARBProc)(::Display*, GLXDrawable, GLXDrawable, GLXContext);
namespace odfaeg {
    namespace window {
        using namespace sf;
        ////////////////////////////////////////////////////////////
        void ensureExtensionsInit(::Display* display, int screen)
        {
            static bool initialized = false;
            if (!initialized)
            {
                initialized = true;
                // We don't check the return value since the extension
                // flags are cleared even if loading fails
                sfglx_LoadFunctions(display, screen);
            }
        }
        ////////////////////////////////////////////////////////////
        GlFunctionPointer GlxContext::getFunction(const char* name)
        {
            return reinterpret_cast<GlFunctionPointer>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>(name)));
        }
        bool GlxContext::ctxErrorOccurred = false;
        // Helper to check for extension string presence.  Adapted from:
        //   http://www.opengl.org/resources/features/OGLextensions/
        bool GlxContext::isExtensionSupported(const char *extList, const char *extension)
        {
          const char *start;
          const char *where, *terminator;

          /* Extension names should not have spaces. */
          where = strchr(extension, ' ');
          if (where || *extension == '\0')
            return false;

          /* It takes a bit of care to be fool-proof about parsing the
             OpenGL extensions string. Don't be fooled by sub-strings,
             etc. */
          for (start=extList;;) {
            where = strstr(start, extension);

            if (!where)
              break;

            terminator = where + strlen(extension);

            if ( where == start || *(where - 1) == ' ' )
              if ( *terminator == ' ' || *terminator == '\0' )
                return true;

            start = terminator;
          }

          return false;
        }
        int GlxContext::ctxErrorHandler(::Display *dpy, XErrorEvent *ev )
        {
            ctxErrorOccurred = true;
            return 0;
        }
        GlxContext::GlxContext() {
            m_display = Display::openDisplay();
            ensureExtensionsInit(m_display, DefaultScreen(m_display));
            m_windowLess = true;
            pbuf = 0;
            ctx = nullptr;
        }
        ////////////////////////////////////////////////////////////
        XVisualInfo GlxContext::selectBestVisual(::Display* display, unsigned int bitsPerPixel, const ContextSettings& settings)
        {
            // Retrieve all the visuals
            int count;
            XVisualInfo* visuals = XGetVisualInfo(display, 0, NULL, &count);
            if (visuals)
            {
                // Evaluate all the returned visuals, and pick the best one
                int bestScore = 0x7FFFFFFF;
                XVisualInfo bestVisual = XVisualInfo();
                for (int i = 0; i < count; ++i)
                {
                    // Check mandatory attributes
                    int doubleBuffer;
                    glXGetConfig(display, &visuals[i], GLX_DOUBLEBUFFER, &doubleBuffer);
                    if (!doubleBuffer)
                        continue;

                    // Extract the components of the current visual
                    int red, green, blue, alpha, depth, stencil, multiSampling, samples, sRgb;
                    glXGetConfig(display, &visuals[i], GLX_RED_SIZE,     &red);
                    glXGetConfig(display, &visuals[i], GLX_GREEN_SIZE,   &green);
                    glXGetConfig(display, &visuals[i], GLX_BLUE_SIZE,    &blue);
                    glXGetConfig(display, &visuals[i], GLX_ALPHA_SIZE,   &alpha);
                    glXGetConfig(display, &visuals[i], GLX_DEPTH_SIZE,   &depth);
                    glXGetConfig(display, &visuals[i], GLX_STENCIL_SIZE, &stencil);
                    glXGetConfig(display, &visuals[i], GLX_SAMPLE_BUFFERS_ARB, &multiSampling);
                    glXGetConfig(display, &visuals[i], GLX_SAMPLES_ARB,        &samples);
                    glXGetConfig(display, &visuals[i], GLX_FRAMEBUFFER_SRGB_CAPABLE_ARB, &sRgb);
                    // TODO: Replace this with proper acceleration detection
                    bool accelerated = true;

                    // Evaluate the visual
                    int color = red + green + blue + alpha;
                    int score = ContextImpl::evaluateFormat(bitsPerPixel, settings, color, depth, stencil, multiSampling ? samples : 0, accelerated, sRgb == True);

                    // If it's better than the current best, make it the new best
                    if (score < bestScore)
                    {
                        bestScore = score;
                        bestVisual = visuals[i];
                    }
                }

                // Free the array of visuals
                XFree(visuals);

                return bestVisual;
            }
            else
            {
                // Should never happen...
                std::cerr << "No GLX visual found. You should check your graphics driver" << std::endl;

                return XVisualInfo();
            }
        }
        void GlxContext::create(ContextSettings& settings, unsigned int width, unsigned int height, IContext* sharedContext) {
            GLXContext shared = (sharedContext != nullptr) ? static_cast<GlxContext*>(sharedContext)->ctx : nullptr;
            XVisualInfo vi = selectBestVisual(m_display, VideoMode::getDesktopMode().bitsPerPixel, settings);
            int versionMajor=0, versionMinor=0, depthBits=0, stencilBits=0, antiAliasingLevel=0;
            // Get a GLXFBConfig that matches the visual
            GLXFBConfig* bestFbc = nullptr;
            // We don't supply attributes to match against, since
            // the visual we are matching against was already
            // deemed suitable in selectBestVisual()
            int nbConfigs = 0;
            GLXFBConfig* configs = glXChooseFBConfig(m_display, DefaultScreen(m_display), NULL, &nbConfigs);
            int bestVersionMajor = 0, bestVersionMinor = 0, bestDepthBits = 0, bestStencilBits = 0, bestAntiAliasingLevel = 0;
            for (int i = 0; configs && (i < nbConfigs); ++i)
            {
                XVisualInfo* visual = glXGetVisualFromFBConfig(m_display, configs[i]);

                if (!visual)
                    continue;

                if (visual->visualid == vi.visualid)
                {
                    bestFbc = &configs[i];
                    XFree(visual);
                    break;
                }
                glXGetConfig(m_display,&vi,GLX_CONTEXT_MAJOR_VERSION_ARB, &versionMajor);
                glXGetConfig(m_display,&vi,GLX_CONTEXT_MINOR_VERSION_ARB, &versionMinor);
                glXGetConfig(m_display,&vi,GLX_DEPTH_SIZE, &depthBits);
                glXGetConfig(m_display,&vi,GLX_STENCIL_SIZE, &stencilBits);
                glXGetConfig(m_display,&vi,GLX_SAMPLES, &antiAliasingLevel);
                if (versionMajor > bestVersionMajor) {
                    bestVersionMajor = versionMajor;
                    this->settings.versionMajor = versionMajor;
                }
                if (versionMinor > bestVersionMinor) {
                    bestVersionMinor = versionMinor;
                    this->settings.versionMinor = versionMinor;
                }
                if (depthBits > bestDepthBits) {
                    bestDepthBits = depthBits;
                    this->settings.depthBits = depthBits;
                }
                if (stencilBits > bestStencilBits) {
                    bestStencilBits = stencilBits;
                    this->settings.stencilBits = stencilBits;
                }
                if (antiAliasingLevel > bestAntiAliasingLevel) {
                    bestAntiAliasingLevel = antiAliasingLevel;
                    this->settings.antiAliasingLevel = antiAliasingLevel;
                }
                XFree(visual);
            }
            m_window = DefaultRootWindow(m_display);
            m_windowLess = true;
            glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
            glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)
            glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB" );
            glXMakeContextCurrentARBProc glXMakeContextCurrentARB = 0;
            glXMakeContextCurrentARB = (glXMakeContextCurrentARBProc)
            glXGetProcAddressARB( (const GLubyte *) "glXMakeContextCurrent");
            // Install an X error handler so the application won't exit if GL 3.0
            // context allocation fails.
            //
            // Note this error handler is global.  All display connections in all threads
            // of a process use the same error handler, so be sure to guard against other
            // threads issuing X commands while this code is running.
            int (*oldHandler)(::Display*, XErrorEvent*) =
              XSetErrorHandler(&ctxErrorHandler);
            // Get the default screen's GLX extension list
            const char *glxExts = glXQueryExtensionsString(m_display,
                                                          DefaultScreen(m_display ) );
            // Check for the GLX_ARB_create_context extension string and the function.
            // If either is not present, use GLX 1.3 context creation method.
            if ( !isExtensionSupported( glxExts, "GLX_ARB_create_context") /*|| !isExtensionSupported( glxExts, "GLX_ARB_make_context_current" )*/ ||
               !glXCreateContextAttribsARB || !glXMakeContextCurrentARB)
            {
                std::cout<<"glXCreateContextAttribsARB() not found"
                " ... using old-style GLX context"<<std::endl;
                ctx = glXCreateContext(m_display, &vi, NULL, GL_TRUE);
            } else {
                int context_attribs[] = {
                        GLX_CONTEXT_MAJOR_VERSION_ARB, (int) settings.versionMajor,
                        GLX_CONTEXT_MINOR_VERSION_ARB, (int) settings.versionMinor,
                        None
                };
                ctx = glXCreateContextAttribsARB(m_display, *bestFbc, shared, True, context_attribs);
                // Sync to ensure any errors generated are processed.
                XSync(m_display, False );
                if ( !ctxErrorOccurred && ctx ) {
                    /* create temporary pbuffer */
                    int pbuffer_attribs[] = {
                            GLX_PBUFFER_WIDTH, (int) width,
                            GLX_PBUFFER_HEIGHT, (int) height,
                            None
                    };
                    pbuf = glXCreatePbuffer(m_display, *bestFbc, pbuffer_attribs);
                    XSync(m_display, False);
                } else
                {
                  // Couldn't create GL 3.0 context.  Fall back to old-style 2.x context.
                  // When a context version below 3.0 is requested, implementations will
                  // return the newest context version compatible with OpenGL versions less
                  // than version 3.0.
                  // GLX_CONTEXT_MAJOR_VERSION_ARB = 1
                  context_attribs[1] = settings.versionMajor;
                  // GLX_CONTEXT_MINOR_VERSION_ARB = 0
                  context_attribs[3] = settings.versionMinor;

                  ctxErrorOccurred = false;
                  std::cout<<"Failed to create GL 3.0 context"
                          " ... using old-style GLX context"<<std::endl;
                  ctx = glXCreateContext(m_display, &vi, shared, GL_TRUE);
                }
                // Sync to ensure any errors generated are processed.
                XSync(m_display, False );

                // Restore the original error handler
                XSetErrorHandler( oldHandler );
                if ( ctxErrorOccurred || !ctx )
                {
                    std::cerr<<"Failed to create an OpenGL context"<<std::endl;
                    exit(1);
                }
                // Verifying that context is a direct context
                if ( ! glXIsDirect (m_display, ctx ) )
                {
                    std::cout<<"Indirect GLX rendering context obtained"<<std::endl;
                }
                else
                {
                    std::cout<<"Direct GLX rendering context obtained"<<std::endl;
                }
            }
        }
        void GlxContext::create(::Window win, IContext* sharedContext) {
            GLXContext shared = (sharedContext != nullptr) ? static_cast<GlxContext*>(sharedContext)->ctx : nullptr;
            m_window = win;
            m_windowLess = false;
            // Retrieve the attributes of the target window
            XWindowAttributes windowAttributes;
            if (XGetWindowAttributes(m_display, m_window, &windowAttributes) == 0)
            {
                std::cerr << "Failed to get the window attributes" << std::endl;
                return;
            }

            // Get its visuals
            XVisualInfo tpl;
            tpl.screen   = DefaultScreen(m_display);
            tpl.visualid = XVisualIDFromVisual(windowAttributes.visual);
            int nbVisuals = 0;
            XVisualInfo* vi = XGetVisualInfo(m_display, VisualIDMask | VisualScreenMask, &tpl, &nbVisuals);
            int versionMajor = 0, versionMinor = 0,  depthBits=0, stencilBits=0, antiAliasingLevel=0;
            // Get a GLXFBConfig that matches the visual
            GLXFBConfig* bestFbc = nullptr;

            // We don't supply attributes to match against, since
            // the visual we are matching against was already
            // deemed suitable in selectBestVisual()
            int nbConfigs = 0;
            GLXFBConfig* configs = glXChooseFBConfig(m_display, DefaultScreen(m_display), NULL, &nbConfigs);
            int bestVersionMajor = 0, bestVersionMinor = 0, bestDepthBits = 0, bestStencilBits = 0, bestAntiAliasingLevel=0;
            for (int i = 0; configs && (i < nbConfigs); ++i)
            {
                XVisualInfo* visual = glXGetVisualFromFBConfig(m_display, configs[i]);

                if (!visual)
                    continue;

                if (visual->visualid == vi->visualid)
                {
                    bestFbc = &configs[i];
                    XFree(visual);
                    break;
                }
                glXGetConfig(m_display,vi,GLX_CONTEXT_MAJOR_VERSION_ARB, &versionMajor);
                glXGetConfig(m_display,vi,GLX_CONTEXT_MINOR_VERSION_ARB, &versionMinor);
                glXGetConfig(m_display,vi,GLX_DEPTH_SIZE, &depthBits);
                glXGetConfig(m_display,vi,GLX_STENCIL_SIZE, &stencilBits);
                glXGetConfig(m_display,vi,GLX_SAMPLES, &antiAliasingLevel);
                if (versionMajor > bestVersionMajor) {
                    bestVersionMajor = versionMajor;
                    settings.versionMajor = versionMajor;
                }
                if (versionMinor > bestVersionMinor) {
                    bestVersionMinor = versionMinor;
                    settings.versionMinor = versionMinor;
                }
                if (depthBits > bestDepthBits) {
                    bestDepthBits = depthBits;
                    settings.depthBits = depthBits;
                }
                if (stencilBits > bestStencilBits) {
                    bestStencilBits = stencilBits;
                    settings.stencilBits = stencilBits;
                }
                if (antiAliasingLevel > bestAntiAliasingLevel) {
                    bestAntiAliasingLevel = antiAliasingLevel;
                    settings.antiAliasingLevel = antiAliasingLevel;
                }
                XFree(visual);
            }
            // Get the default screen's GLX extension list
            const char *glxExts = glXQueryExtensionsString(m_display,
                                                          DefaultScreen(m_display ) );
            // NOTE: It is not necessary to create or make current to a context before
            // calling glXGetProcAddressARB
            glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
            glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)
            glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB" );
            // Install an X error handler so the application won't exit if GL 3.0
            // context allocation fails.
            //
            // Note this error handler is global.  All display connections in all threads
            // of a process use the same error handler, so be sure to guard against other
            // threads issuing X commands while this code is running.
            int (*oldHandler)(::Display*, XErrorEvent*) =
              XSetErrorHandler(&ctxErrorHandler);
            // Check for the GLX_ARB_create_context extension string and the function.
            // If either is not present, use GLX 1.3 context creation method.
            if ( !isExtensionSupported( glxExts, "GLX_ARB_create_context" ) ||
               !glXCreateContextAttribsARB )
            {
                 std::cout<<"glXCreateContextAttribsARB() not found"
                       " ... using old-style GLX context"<<std::endl;
                 ctx = glXCreateNewContext(m_display, *bestFbc, GLX_RGBA_TYPE, shared, True );
            }
            // If it does, try to get a GL 3.0 context!
            else
            {
                int context_attribs[] =
                {
                    GLX_CONTEXT_MAJOR_VERSION_ARB, (int) settings.versionMajor,
                    GLX_CONTEXT_MINOR_VERSION_ARB, (int) settings.versionMinor,
                    //GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
                    None
                };
                ctx = glXCreateContextAttribsARB(m_display, *bestFbc, shared,
                                                  True, context_attribs );

                // Sync to ensure any errors generated are processed.
                XSync(m_display, False );
                if ( !ctxErrorOccurred && ctx ) {
                    std::cout<<"Created GL 3.0 context"<<std::endl;
                } else
                {
                  // Couldn't create GL 3.0 context.  Fall back to old-style 2.x context.
                  // When a context version below 3.0 is requested, implementations will
                  // return the newest context version compatible with OpenGL versions less
                  // than version 3.0.
                  // GLX_CONTEXT_MAJOR_VERSION_ARB = 1
                  context_attribs[1] = settings.versionMajor;
                  // GLX_CONTEXT_MINOR_VERSION_ARB = 0
                  context_attribs[3] = settings.versionMinor;

                  ctxErrorOccurred = false;

                  std::cout<<"Failed to create GL 3.0 context"
                          " ... using old-style GLX context"<<std::endl;
                  ctx = glXCreateContextAttribsARB(m_display, *bestFbc, shared,
                                                    True, context_attribs );
                }
                // Sync to ensure any errors generated are processed.
                XSync(m_display, False );

                // Restore the original error handler
                XSetErrorHandler( oldHandler );

                if ( ctxErrorOccurred || !ctx )
                {
                    std::cerr<<"Failed to create an OpenGL context"<<std::endl;
                    exit(1);
                }
                // Verifying that context is a direct context
                if ( ! glXIsDirect (m_display, ctx ) )
                {
                    std::cout<<"Indirect GLX rendering context obtained"<<std::endl;
                }
                else
                {
                    std::cout<<"Direct GLX rendering context obtained"<<std::endl;
                }
            }
        }
        bool GlxContext::setActive(bool current) {
            if (!ctx)
                return false;
            bool result;
            if (current)
            {
                if (m_windowLess) {
                    if (pbuf) {
                        /* try to make it the current context */
                        result = glXMakeContextCurrent(m_display, pbuf, pbuf, ctx);
                        if ( !result ) {
                                /* some drivers does not support context without default framebuffer, so fallback on
                                 * using the default window.
                                 */
                                result = glXMakeContextCurrent(m_display, DefaultRootWindow(m_display), DefaultRootWindow(m_display), ctx);
                                if ( !result){
                                        std::cerr<<"failed to make current"<<std::endl;
                                        exit(1);
                                }
                        }
                    } else {
                        result = glXMakeCurrent(m_display, m_window, ctx );
                    }
                } else {
                    result = glXMakeCurrent(m_display, m_window, ctx );
                }
            } else {
                result = glXMakeCurrent(m_display, None, NULL);
            }
            return result;
        }
        const ContextSettings& GlxContext::getSettings() const {
            return settings;
        }
        void GlxContext::display() {
            if (pbuf)
                glXSwapBuffers(m_display, pbuf);
            else if (m_window)
                glXSwapBuffers(m_display, m_window);
        }
        void GlxContext::setVerticalSyncEnabled(bool enable) {
            //For later.
        }
        GlxContext::~GlxContext() {
            if (pbuf)
            {
                glXDestroyPbuffer(m_display, pbuf);
            }
            if (ctx) {
                glXDestroyContext(m_display, ctx );
            }
            Display::closeDisplay(m_display);
        }
    }
}
