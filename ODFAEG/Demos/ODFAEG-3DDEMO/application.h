
#include "odfaeg/Core/application.h"
#include "odfaeg/Graphics/3D/cube.h"
#include "odfaeg/Graphics/map.h"
#include "odfaeg/Graphics/tGround.h"
#include "odfaeg/Graphics/entitiesUpdater.h"
#include "odfaeg/Graphics/billBoard.h"
#include "odfaeg/Core/world.h"
#include <SFML/Window/Event.hpp>
class MyAppli : public odfaeg::core::Application {
public :
    enum TEXTURES {
        GRASS, PARTICLE
    };
    MyAppli(odfaeg::math::Vec2f size, std::string title);
    void onLoad();
    void onInit();
    void onRender(odfaeg::graphic::FastRenderComponentManager* frcm);
    void onDisplay(odfaeg::graphic::RenderWindow* window);
    void onUpdate (sf::Event& event);
    void onExec();
private :
    odfaeg::graphic::Entity* cube;
    odfaeg::graphic::Map* theMap;
    odfaeg::graphic::BillBoard* billboard;
    odfaeg::physic::ParticleSystem ps;
    float speed, sensivity;
    int oldX, oldY;
    bool verticalMotionActive;
    int verticalMotionDirection;
    sf::Clock clock, clock2;
    sf::Time timeBeforeStoppingVerticalMotion;
    odfaeg::graphic::EntitiesUpdater *eu;
    odfaeg::core::ResourceCache<> cache;
    odfaeg::graphic::BigTile* heightmap;
};
