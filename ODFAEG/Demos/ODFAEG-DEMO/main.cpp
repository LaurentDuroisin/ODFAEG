#include "application.h"

using namespace odfaeg::core;
using namespace odfaeg::math;
using namespace odfaeg::physic;
using namespace odfaeg::graphic;
using namespace odfaeg::audio;
int main(int argc, char* argv[])
{
    EXPORT_CLASS_GUID(BoundingVolumeBoundingBox, BoundingVolume, BoundingBox)
    EXPORT_CLASS_GUID(EntityTile, Entity, Tile)
    EXPORT_CLASS_GUID(EntityTile, Entity, BigTile)
    EXPORT_CLASS_GUID(EntityShadowTile, Entity, g2d::ShadowTile)
    EXPORT_CLASS_GUID(EntityShadowWall, Entity, g2d::ShadowWall)
    EXPORT_CLASS_GUID(EntityWall, Entity, g2d::Wall)
    EXPORT_CLASS_GUID(EntityDecor, Entity, g2d::Decor)
    EXPORT_CLASS_GUID(EntityAnimation, Entity, Anim)
    /*RenderWindow window(sf::VideoMode(800, 600), "Test");
    Texture tex;
    tex.loadFromFile("tilesets/herbe.png");
    Tile tile(&tex, Vec3f(0, 0, 0), Vec3f(120, 60, 0),sf::IntRect(0, 0, 100, 50));
    FastRenderComponent rc (window, 0, "E_TILE", false);
    std::vector<Entity*> entities = {&tile};


    while (window.isOpen()) {
        window.clear();
        rc.clear();
        rc.loadEntitiesOnComponent(entities);
        rc.drawNextFrame();
        window.draw(rc);
        window.display();
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }
    return 0;*/
    MyAppli app(sf::VideoMode(800, 600), "Test odfaeg");
    return app.exec();
}




