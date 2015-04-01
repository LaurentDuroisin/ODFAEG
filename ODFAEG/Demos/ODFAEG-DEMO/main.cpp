#include "application.h"
#include "odfaeg/Window/x11Window.hpp"
using namespace odfaeg::core;
using namespace odfaeg::math;
using namespace odfaeg::physic;
using namespace odfaeg::graphic;
using namespace odfaeg::window;
using namespace odfaeg::audio;
int main(int argc, char* argv[])
{
    /*RenderWindow window (sf::VideoMode (800, 600), "Test", sf::Style::Default, sf::ContextSettings(0, 0, 4, 3, 3));
    window.getView().move(400, 300, 0);
    Texture tex;
    RectangleShape rs(Vec3f(100, 100, 0));
    rs.move(Vec3f(100, 100, 0));
    tex.loadFromFile("tilesets/herbe.png");
    Tile tile (&tex, Vec3f(0, 0, 0), Vec3f(100, 50, 0), sf::IntRect(0, 0, 100, 50));
    //tile.setColor(sf::Color(255, 0, 0, 128));
    Tile tile2 (&tex, Vec3f(10, 10, 0), Vec3f(100, 50, 0), sf::IntRect(0, 0, 100, 50));
    //tile2.setColor(sf::Color(0, 255, 0, 128));
    std::vector<Entity*> entities = {&tile, &tile2};
    FastRenderComponent frc(window,0,"E_TILE",false);
    frc.clear();
    frc.loadEntitiesOnComponent(entities);
    //frc.draw(rs);
    frc.drawNextFrame();
    while (window.isOpen()) {
        window.clear();
        window.draw(frc);
        window.display();
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
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




