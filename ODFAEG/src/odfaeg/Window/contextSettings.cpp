#include "../../../include/odfaeg/Window/contextSettings.hpp"
namespace odfaeg {
    namespace window {
        ContextSettings::ContextSettings () {
            versionMajor = 3;
            versionMinor = 0;
            depthBits = 0;
            stencilBits = 0;
            antiAliasingLevel = 4;
        }
        ContextSettings::ContextSettings (unsigned int depthBits, unsigned int stencilBits, unsigned int antiAliasingLevel, unsigned  int versionMajor, unsigned int versionMinor)  :
        versionMajor(versionMajor), versionMinor(versionMinor), depthBits(depthBits), stencilBits(stencilBits), antiAliasingLevel (antiAliasingLevel) {
        }
    }
}
