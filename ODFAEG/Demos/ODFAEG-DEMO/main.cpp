#include "application.h"
using namespace odfaeg::core;
using namespace odfaeg::math;
using namespace odfaeg::physic;
using namespace odfaeg::graphic;
using namespace odfaeg::audio;

int main()
{
    /*sf::RenderWindow window(sf::VideoMode(800, 600), "OpenGL");
    sf::RenderWindow window1(sf::VideoMode(500, 100), "test");
    sf::RenderWindow window2(sf::VideoMode(500, 100), "test2");

    bool once = true;

    while (window.isOpen())
    {
        sf::Event event;
        while(window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        while(window1.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window1.close();
        }
        while(window2.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window2.close();
        }

        if (once) {
            window1.setVisible(false);
            window2.setVisible(false);
            once = false;
        }

        window.clear();
        window.display();
    }
    return 0;*/
    EXPORT_CLASS_GUID(BoundingVolumeBoundingBox, BoundingVolume, BoundingBox)
    EXPORT_CLASS_GUID(EntityTile, Entity, Tile)
    EXPORT_CLASS_GUID(EntityTile, Entity, BigTile)
    EXPORT_CLASS_GUID(EntityShadowTile, Entity, g2d::ShadowTile)
    EXPORT_CLASS_GUID(EntityShadowWall, Entity, g2d::ShadowWall)
    EXPORT_CLASS_GUID(EntityWall, Entity, g2d::Wall)
    EXPORT_CLASS_GUID(EntityDecor, Entity, g2d::Decor)
    EXPORT_CLASS_GUID(EntityAnimation, Entity, Anim)
    MyAppli app(sf::VideoMode(800, 600, 32), "Test odfaeg");
    return app.exec();
}




