#include "../../../include/odfaeg/Physics/collisionResultSet.hpp"
namespace odfaeg {
    namespace physic {
        std::vector<graphic::Entity*> CollisionResultSet::entities = std::vector<graphic::Entity*>();
        std::vector<math::Vec3f> CollisionResultSet::mtus = std::vector<math::Vec3f>();
    }
}
