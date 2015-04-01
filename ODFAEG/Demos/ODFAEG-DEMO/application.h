
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
#include "odfaeg/Graphics/2D/decor.h"
#include "odfaeg/Graphics/anim.h"
#include "odfaeg/Graphics/2D/ambientLight.h"
#include "odfaeg/Graphics/2D/ponctualLight.h"
#include "odfaeg/Graphics/2D/wall.h"
#include "odfaeg/Graphics/tGround.h"
#include "odfaeg/Core/command.h"
#include "odfaeg/Graphics/entitiesUpdater.h"
#include "odfaeg/Graphics/animationUpdater.h"
#include "caracter.h"
#include "odfaeg/Graphics/billBoard.h"
#include "odfaeg/Physics/emmiters.h"
#include "odfaeg/Audio/player.h"
#include "odfaeg/Graphics/GUI/textArea.hpp"
#include "odfaeg/Graphics/GUI/optionPane.hpp"
#include <fstream>
class MyAppli : public odfaeg::core::Application, public odfaeg::graphic::gui::FocusListener {
private :
    const float speed = 0.2f;
    odfaeg::graphic::EntitiesUpdater *eu;
    odfaeg::graphic::AnimUpdater *au;
    bool running;
    odfaeg::graphic::g2d::Wall *w;
    Caracter* caracter;
    sf::Keyboard::Key actualKey, previousKey;
    std::vector<odfaeg::graphic::Tile*> tiles;
    std::vector<odfaeg::graphic::Tile*> walls;
    odfaeg::graphic::Map* theMap;
    odfaeg::graphic::g2d::PonctualLight* light2;
    odfaeg::graphic::Shader shader;
    odfaeg::core::ResourceCache<> cache;
    unsigned int fpsCounter;
    bool day;
    odfaeg::physic::UniversalEmitter emitter;
    odfaeg::physic::ParticleSystem ps;
    odfaeg::audio::Player player;
    odfaeg::audio::Player pfire;
    odfaeg::graphic::gui::OptionPane* op;
    odfaeg::graphic::VertexArray point;
public :
    MyAppli(sf::VideoMode wm, std::string title);
    void gaignedFocus(odfaeg::graphic::gui::TextArea* textArea);
    void keyHeldDown (sf::Keyboard::Key key);
    void leftMouseButtonPressed(sf::Vector2f mousePos);
    bool mouseInside (sf::Vector2f mousePos);
    void onMouseInside (sf::Vector2f mousePos);
    void onLoad();
    void onInit ();
    void onRender(odfaeg::graphic::FastRenderComponentManager *cm);
    void onDisplay(odfaeg::graphic::RenderWindow* window);
    void onUpdate (sf::Event& event);
    void onExec ();
};
#endif // MY_APPLI
