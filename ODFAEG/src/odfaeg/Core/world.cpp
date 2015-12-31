#include "../../../include/odfaeg/Core/world.h"
#include "../../../include/odfaeg/Core/application.h"
#include "../../../include/odfaeg/Graphics/animationUpdater.h"
#include "../../../include/odfaeg/Graphics/entitiesUpdater.h"
#include "../../../include/odfaeg/Graphics/map.h"


namespace odfaeg {
    namespace core {
        using namespace std;
        vector<graphic::EntityManager*> World::ems = vector<graphic::EntityManager*>();
        vector<EntitySystem*> World::eus = vector<EntitySystem*>();
        vector<Timer*> World::aus = vector<Timer*>();
        graphic::EntityManager* World::currentEntityManager = nullptr;
    }
}
