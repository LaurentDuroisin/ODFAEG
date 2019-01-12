#include "application.h"
#include "odfaeg/Core/clock.h"
#include "odfaeg/Graphics/GUI/label.hpp"
using namespace odfaeg::core;
using namespace odfaeg::math;
using namespace odfaeg::physic;
using namespace odfaeg::graphic;
using namespace odfaeg::audio;
using namespace sorrok;
using namespace std;
class SimpleAppli : public Application {
    public :
    SimpleAppli(sf::VideoMode mode, std::string title) : Application(mode, title) {
    }
    void onLoad() {}
    void onInit () {
        Font* font = new Font();
        font->loadFromFile("fonts/FreeSerif.ttf");
        wResuHero = new RenderWindow (sf::VideoMode(400, 300), "Create ODFAEG Application", sf::Style::Titlebar, sf::ContextSettings(0, 0, 4, 3, 0));
        gui::Label* label = new gui::Label(*wResuHero, Vec3f(0, 0, 0), Vec3f(200, 50, 0),font,"5",15);
        wResuHero->setVisible(true);
        getRenderComponentManager().addComponent(label);
        gui::Button* button = new gui::Button(Vec3f(0, 200, 0), Vec3f(400, 100, 0), font,"Respawn", 15, *wResuHero);
        getRenderComponentManager().addComponent(button);
        wResuHero->setPosition(sf::Vector2i(500, 400));
        addWindow(wResuHero);
    }
    void onRender(odfaeg::graphic::RenderComponentManager *cm) {}
    void onDisplay(odfaeg::graphic::RenderWindow* window) {}
    void onUpdate (odfaeg::graphic::RenderWindow* window, sf::Event& event) {
        if (window == &getRenderWindow() && event.type == sf::Event::Closed)
           stop();
        if (event.type == sf::Event::KeyPressed)
            wResuHero->setVisible(false);
    }
    void onExec () {}
    private :
    RenderWindow* wResuHero;
};
int main(int argc, char* argv[])
{
    EXPORT_CLASS_GUID(BoundingVolumeBoundingBox, BoundingVolume, BoundingBox)
    EXPORT_CLASS_GUID(EntityTile, Entity, Tile)
    EXPORT_CLASS_GUID(EntityTile, Entity, BigTile)
    EXPORT_CLASS_GUID(EntityWall, Entity, g2d::Wall)
    EXPORT_CLASS_GUID(EntityDecor, Entity, g2d::Decor)
    EXPORT_CLASS_GUID(EntityAnimation, Entity, Anim)
    EXPORT_CLASS_GUID(EntityHero, Entity, Hero)
    MyAppli app(sf::VideoMode(800, 600, 32), "Test odfaeg");
    return app.exec();
    /*impleAppli app(sf::VideoMode(800, 600), "test");
    return app.exec();*/
}




