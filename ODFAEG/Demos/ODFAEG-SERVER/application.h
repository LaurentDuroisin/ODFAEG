
#include <iostream>
#include <string>
#include <vector>
// *** END ***
#ifndef MY_APPLI
#define MY_APPLI
#include "odfaeg/Core/application.h"
#include "odfaeg/Graphics/convexShape.h"
#include "odfaeg/Graphics/rectangleShape.h"
#include "odfaeg/Graphics/circleShape.h"
#include "odfaeg/Graphics/tile.h"
#include "odfaeg/Core/world.h"
#include "odfaeg/Graphics/map.h"
#include "odfaeg/Graphics//2D/decor.h"
#include "odfaeg/Graphics/anim.h"
#include "odfaeg/Graphics/2D/ambientLight.h"
#include "odfaeg/Graphics/2D/ponctualLight.h"
#include "odfaeg/Graphics/2D/wall.h"
#include "odfaeg/Graphics/tGround.h"
#include "odfaeg/Core/actionMap.h"
#include "odfaeg/Graphics/entitiesUpdater.h"
#include "odfaeg/Graphics/animationUpdater.h"
#include "odfaeg/Network/network.h"
#include "caracter.h"
#include "monster.h"
#include <fstream>
class MyAppli : public odfaeg::core::Application {
private :
    const float speed = 0.2f;
    bool running;
    odfaeg::graphic::g2d::Wall *w;
    Caracter* caracter;
    sf::Keyboard::Key actualKey, previousKey;
    std::vector<odfaeg::graphic::Tile*> tiles;
    std::vector<odfaeg::graphic::Tile*> walls;
    odfaeg::graphic::Map* theMap;
    odfaeg::graphic::g2d::PonctualLight* light2;
    sf::Int64 sync_time;
    sf::Time timeBtwnTwoSync = sf::seconds(1);
    sf::Int64 last_srv_time;
public :
    MyAppli();
    void onLoad();
    void onInit ();
    void onExec ();
};
#endif // MY_APPLI

