#ifndef ODFAEG_VBO_HPP
#define ODFAEG_VBO_HPP
#include <SFML/Window/GlResource.hpp>
#include <vector>
#include "export.hpp"
namespace odfaeg {
    namespace graphic {
        class ODFAEG_GRAPHICS_API VBO : public sf::GlResource {
        friend class VertexArray;
        public:
            VBO();
            void create (const void* data, const unsigned int dataSize);
            bool update (const void* data, const unsigned int dataSize);
            static bool isAvailable();
            virtual ~VBO();
        private :
            VBO(const VBO&);
            VBO& operator=(const VBO&);
            unsigned int m_vbo;
        };
    }
}
#endif // ODFAEG_VBO_HPP
