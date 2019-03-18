#include "../../../include/odfaeg/Window/contextSettings.hpp"
namespace odfaeg {
    namespace window {
        ContextSettings::ContextSettings () {
            versionMajor = 3;
            versionMinor = 0;
            depthBits = 0;
            stencilBits = 0;
            antiAliasingLevel = 4;
            attributeFlags = Default;
            sRgbCapable = false;
        }
        ContextSettings::ContextSettings (unsigned int depthBits, unsigned int stencilBits, unsigned int antiAliasingLevel, unsigned  int versionMajor, unsigned int versionMinor, sf::Uint32 attributes, bool sRgbCapable)  :
        versionMajor(versionMajor), versionMinor(versionMinor), depthBits(depthBits), stencilBits(stencilBits), antiAliasingLevel (antiAliasingLevel), attributeFlags(attributes), sRgbCapable(sRgbCapable) {
        }
    }
}
