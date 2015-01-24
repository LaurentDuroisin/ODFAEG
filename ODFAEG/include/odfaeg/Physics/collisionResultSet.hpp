#ifndef ODFAEG_COLLISION_RESULT_SET
#define ODFAEG_COLLISION_RESULT_SET
#include <vector>
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
                graphic::Entity* entity = entities.back();
                entities.pop_back();
                return entity;
            }
            private :
            static std::vector<graphic::Entity*> entities;
        };
    }
}
#endif
