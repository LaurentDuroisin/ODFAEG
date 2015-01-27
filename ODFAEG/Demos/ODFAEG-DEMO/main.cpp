#include "application.h"
#include <SDL2/SDL.h>
using namespace odfaeg::core;
using namespace odfaeg::math;
using namespace odfaeg::physic;
using namespace odfaeg::graphic;
using namespace odfaeg::audio;

/*void renderingThread(sf::RenderWindow* window)
{
    // the rendering loop
    while (window->isOpen())
    {
        // draw...

        // end the current frame
        window->display();
    }
}*/


int main()
{
    // create the window (remember: it's safer to create it in the main thread due to OS limitations)
    /*RenderWindow window(sf::VideoMode(800, 600), "OpenGL");

    // deactivate its OpenGL context
    //window.setActive(false);

    // launch the rendering thread
    /*sf::Thread thread(&renderingThread, &window);
    thread.launch();*/
    /*Batcher batcher;
    // the event/logic/whatever loop
    while (window.isOpen())
    {
        sf::Event event;
        while(window.pollEvent(event)) {
              if (event.type == sf::Event::Closed)
                    window.close();
        }
        Tile tile(nullptr, Vec3f(0, 0, 0), Vec3f(100, 100, 0), sf::IntRect(0, 0, 0 ,0));
        Tile tile2(nullptr, Vec3f(0, 0, 0), Vec3f(100, 100, 0), sf::IntRect(0, 0, 0 ,0));
        tile.move(Vec3f(100, 100, 0));
        batcher.clear();
        batcher.addFace(tile.getFaces()[0]);
        batcher.addFace(tile2.getFaces()[0]);
        VertexArray va = batcher.getInstances()[0]->getVertexArray();
        window.clear();
        window.draw(va);
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




