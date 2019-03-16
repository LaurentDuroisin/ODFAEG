#ifndef ODFAEG_MESH_HPP
#define ODFAEG_MESH_HPP
#include "entity.h"
namespace odfaeg {
    namespace graphic {
        class Mesh : public Entity {
            public :
            Mesh();
            Mesh(math::Vec3f position, math::Vec3f size, std::string type);
            bool operator==(Entity& other);
            bool isAnimated() const;
            bool isModel() const;
            bool selectable() const;
            bool isLight() const;
            bool isShadow() const;
            bool isLeaf() const;
            void onDraw(RenderTarget& target, RenderStates states);
            template <typename Archive>
            void vtserialize(Archive& ar) {
                Entity::vtserialize(ar);
            }
        };
    }
}
#endif
