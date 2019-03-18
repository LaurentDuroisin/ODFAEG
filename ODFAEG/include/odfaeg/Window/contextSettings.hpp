#ifndef ODFAEG_CONTEXT_SETTINGS_HPP
#define ODFAEG_CONTEXT_SETTINGS_HPP
#include <SFML/System.hpp>
namespace odfaeg {
    namespace window {
        class ContextSettings {
            public :
            ////////////////////////////////////////////////////////////
            /// \brief Enumeration of the context attribute flags
            ///
            ////////////////////////////////////////////////////////////
            enum Attribute
            {
                Default = 0,      ///< Non-debug, compatibility context (this and the core attribute are mutually exclusive)
                Core    = 1 << 0, ///< Core attribute
                Debug   = 1 << 2  ///< Debug attribute
            };
            ContextSettings();
            ContextSettings (unsigned int depthBits, unsigned int stencilBits, unsigned int antiAliasingLevelunsigned, unsigned int versionMajor, unsigned int versionMinor, unsigned int attribute = Default, bool sRgbCapable=false);
            unsigned int versionMajor, versionMinor, depthBits, stencilBits, antiAliasingLevel;
            bool sRgbCapable;
            sf::Uint32       attributeFlags;
        };
    }
}
#endif

