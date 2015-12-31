#include "../../../../include/odfaeg/Graphics/2D/wall.h"
namespace odfaeg {
    namespace graphic {
        namespace g2d {

            Wall::Wall (Tile *tile, Light *light) : Model (tile->getPosition(),tile->getSize(), tile->getOrigin(), "E_WALL")  {
                addChild(tile);
                tile->setParent(this);
                float sy = light->getHeight() / (light->getHeight() * 0.75f);
                shadowScale = math::Vec3f(1.f, sy, 1.f);
                setShadowScale(shadowScale);
            }
            bool Wall::isLeaf() const {
                return false;
            }
            void Wall::onMove(math::Vec3f& t) {
                Entity::onMove(t);
            }
            bool Wall::operator== (Entity &other) {
                if (other.getType() != "E_WALL")
                    return false;
                return true;
            }
            bool Wall::selectable () const {
                return true;
            }
            bool Wall::isLight() const {
                return false;
            }
            bool Wall::isShadow() const {
                return false;
            }
        }
    }
}



