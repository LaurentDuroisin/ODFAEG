#include "../../../include/odfaeg/Driver/frameBuffer.hpp"
namespace odfaeg {
    namespace driver {
        FrameBuffer::FrameBuffer(unsigned int width, unsigned int height) : pixels (width * height) {

        }
        sf::Color& FrameBuffer::operator(unsigned int index) {
            if (index >= 0 && index < pixels.size())
                return pixels[i];
            }
            throw core::Error(60, "frame buffer : index out of range", 0);
        }
    }
}
