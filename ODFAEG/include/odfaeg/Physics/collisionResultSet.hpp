#ifndef ODFAEG_COLLISION_RESULT_SET
#define ODFAEG_COLLISION_RESULT_SET
#include <vector>
#include "../Math/vec4.h"
namespace odfaeg {
    namespace graphic {
        class Entity;
    }
    namespace physic {
        class CollisionResultSet {
            public :
            static void pushCollisionEntity(graphic::Entity* entity) {
                entities.push_back(entity);
            }
            static graphic::Entity* popCollisionEntity() {
                graphic::Entity* entity = nullptr;
                if (entities.size() > 0) {
                    entity = entities.back();
                    entities.pop_back();
                }
                return entity;
            }
            static void pushCollisionMTU(math::Vec3f mtu) {
                mtus.push_back(mtu);
            }
            static math::Vec3f popCollisionMTU () {
                math::Vec3f mtu;
                if (mtus.size() > 0) {
                    mtu = mtus.back();
                    mtus.pop_back();
                }
                return mtu;
            }
            private :
            static std::vector<graphic::Entity*> entities;
            static std::vector<math::Vec3f> mtus;
        };
    }
}
#endif
