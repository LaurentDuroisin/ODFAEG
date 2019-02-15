#ifndef ODFAEG_CONTEXT_SETTINGS_HPP
#define ODFAEG_CONTEXT_SETTINGS_HPP
namespace odfaeg {
    namespace window {
        class ContextSettings {
            public :
            ContextSettings();
            ContextSettings (unsigned int depthBits, unsigned int stencilBits, unsigned int antiAliasingLevelunsigned, unsigned int versionMajor, unsigned int versionMinor);
            unsigned int versionMajor, versionMinor, depthBits, stencilBits, antiAliasingLevel;
            bool sRgbCapable;
        };
    }
}
#endif

