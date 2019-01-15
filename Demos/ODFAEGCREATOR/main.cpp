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
    RenderWindow window(sf::VideoMode(1200, 700), "Scissor test");
    window.getView().move(600, 350, 0);
    RectangleShape rect(Vec3f(1200, 700, 0));
    RectangleShape rect2(Vec3f(200, 700, 0));
    rect2.setPosition(Vec3f(0, 10, 0));
    std::cout<<"rect pos : "<<rect2.getPosition()<<" rect size : "<<rect2.getSize()<<std::endl;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        window.clear();

        //window.draw(rect);
        glEnable(GL_SCISSOR_TEST);
        glScissor(0, -10, 200, 700);
        window.draw(rect2);
        glDisable(GL_SCISSOR_TEST);
        window.display();
    }
    return 0;
}
