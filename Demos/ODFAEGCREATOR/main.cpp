#include "application.hpp"
using namespace odfaeg::graphic::gui;
using namespace odfaeg::graphic;
using namespace odfaeg::physic;
using namespace odfaeg::math;
int main(int argc, char* argv[]) {
    EXPORT_CLASS_GUID(BoundingVolumeBoundingBox, BoundingVolume, BoundingBox)
    EXPORT_CLASS_GUID(ShapeRectShape, Shape, RectangleShape)
    ODFAEGCreator app(sf::VideoMode(1200,700),"ODFAEG Creator");
    return app.exec();
    /*RenderWindow window(sf::VideoMode(1200, 700), "Scissor test");
    RenderWindow window2(sf::VideoMode(800, 600), "SubWindow");
    window.getView().move(600, 350, 0);
    window2.getView().move(400, 300, 0);
    Panel p1 (window,Vec3f(0, 0, 0), Vec3f(1200, 700, 0));
    p1.setBackgroundColor(sf::Color::Red);
    Panel p2 (window2,Vec3f(0, 0, 0),Vec3f(800, 600, 0));
    p2.setBackgroundColor(sf::Color::Blue);
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                window2.close();
            }
        }
        window2.clear();
        window2.draw(p2);
        window.clear();
        window.draw(p1);
        window.display();
        window2.display();
    }
    return 0;*/
}
