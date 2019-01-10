#include "application.h"
using namespace odfaeg::physic;
using namespace odfaeg::math;
using namespace odfaeg::graphic;
int main(int argc, char* argv[]) {
    ODFAEGCreator app(sf::VideoMode(1200, 700), "ODFAEG Creator");
    return app.exec();
    /*RenderWindow rw(sf::VideoMode(800, 600), "Test");
    RectangleShape rect(Vec3f(10, 75, 0));
    rect.setPosition(Vec3f(790, 60, 0));
    rw.getView().move(400, 300, 0);
    while (rw.isOpen()) {
        sf::Event event;
        while (rw.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                rw.close();
            }
        }
        rw.clear();
        rw.draw(rect);
        rw.display();
    }*/
    /*odfaeg::graphic::Transformable transform;
    transform.setSize(Vec3f(400, 480, 0));
    transform.setScale(Vec3f(400 / 400, 19.2 / 480, 0));
    std::cout<<transform.getSize()<<std::endl;*/
}
