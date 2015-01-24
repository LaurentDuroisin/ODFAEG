#include "../../../include/odfaeg/Graphics/shadow.h"
namespace odfaeg {
    namespace graphic {
        using namespace sf;
        using namespace std;
        /*Crée une ombre à partir de la lumière (light) et le tile (tile.)
        */
        Shadow::Shadow() : Entity(math::Vec3f(0, 0, 0), math::Vec3f(1, 1, 1), math::Vec3f(0.5f, 0.5f, 0.5f),"E_SHADOW") {
        }
        Shadow::Shadow (const Light &light, Entity *entity, string type) : Entity (entity->getPosition(), entity->getSize(), entity->getSize() * 0.5f, type) {

            entityId = &entity->getId();
            lightId = &(const_cast<Light&> (light).getLightId());
        }
        int& Shadow::getLightId () {
            return *lightId;
        }
        int& Shadow::getEntityId () {
            return *entityId;
        }
    }
}







