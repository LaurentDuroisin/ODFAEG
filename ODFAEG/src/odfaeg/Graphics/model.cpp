#include "../../../include/odfaeg/Graphics/model.h"
namespace odfaeg {
    namespace graphic {
        Model::Model (math::Vec3f position, math::Vec3f size, math::Vec3f origin, std::string type, Entity *parent)
            : Entity(position, size, origin, type, parent) {
        }
        bool Model::isModel() const {
            return true;
        }
        bool Model::isAnimated() const {
            return false;
        }
        bool Model::isLeaf() const {
            return false;
        }
    }
}

