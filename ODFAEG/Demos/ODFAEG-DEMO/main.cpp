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

class ButtonEventTest : public gui::ActionListener {
    void actionPerformed(gui::Button* button) {
        std::cout<<"clicked on the button!"<<std::endl;
    }
};
int main()
{
    // create the window (remember: it's safer to create it in the main thread due to OS limitations)
    //RenderWindow window(sf::VideoMode(800, 600), "OpenGL");

    // deactivate its OpenGL context
    //window.setActive(false);

    // launch the rendering thread
    /*sf::Thread thread(&renderingThread, &window);
    thread.launch();*/
    //Batcher batcher;
    // the event/logic/whatever loop
    /*Font font;
    font.loadFromFile("fonts/FreeSerif.ttf");
    gui::Button button(Vec3f(100, 100, 0), Vec3f(100, 50, 0), &font, "Test", window);
    button.addActionListener(new ButtonEventTest());
    window.getView().move(window.getSize().x * 0.5f, window.getSize().y * 0.5f, 0);
    while (window.isOpen())
    {
        sf::Event event;
        while(window.pollEvent(event)) {
              button.getListener().pushEvent(event);
              if (event.type == sf::Event::Closed)
                    window.close();
        }
        button.getListener().processEvents();
        window.clear();
        window.draw(button);
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




