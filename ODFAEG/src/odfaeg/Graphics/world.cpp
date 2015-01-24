#include "../../../include/odfaeg/Graphics/world.h"
#include "../../../include/odfaeg/Core/application.h"
#include "../../../include/odfaeg/Graphics/animationUpdater.h"
#include "../../../include/odfaeg/Graphics/entitiesUpdater.h"
#include "../../../include/odfaeg/Graphics/map.h"


namespace odfaeg {
    namespace graphic {
        using namespace std;
        vector<EntityManager*> World::ems = vector<EntityManager*>();
        vector<core::EntitySystem*> World::eus = vector<core::EntitySystem*>();
        vector<core::Timer*> World::aus = vector<core::Timer*>();
        EntityManager* World::currentEntityManager = nullptr;
    }
}
