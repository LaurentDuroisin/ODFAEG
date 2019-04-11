#ifndef ODFAEG_FRAMEBUFFER_HPP
#define ODFAEG_FRAMEBUFFER_HPP
#include "../../../include/odfaeg/Core/erreur.h"
namespace odfaeg {
    namespace driver {
        class FrameBuffer {
            public :
            FrameBuffer (unsigned int width, unsigned int height);
            sf::Color& operator[](unsigned int index);
            private :
            std::vector<sf::Color> pixels;
        }
    }
}
#endif // ODFAEG_FRAMEBUFFER_HPP
