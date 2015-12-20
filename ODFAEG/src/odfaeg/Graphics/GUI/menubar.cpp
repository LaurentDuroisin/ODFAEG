#include "../../../../include/odfaeg/Graphics/GUI/menubar.hpp"
namespace odfaeg {
    namespace graphic {
        namespace gui {
            MenuBar::MenuBar(RenderWindow& rw) :
                LightComponent(math::Vec3f(0, 0, 0), math::Vec3f(rw.getSize().x+20, 20, 0), math::Vec3f(0, 0, 0), false),
                window(rw) {
                rect = RectangleShape(math::Vec3f(rw.getSize().x+20, 20, 0));
                background = sf::Color(50, 50, 50);
            }
            void MenuBar::addMenu(Menu* menu) {
                unsigned int posX = 0;
                for (unsigned int i = 0; i < menus.size(); i++) {
                    posX += menus[i]->getSize().x;
                }
                menu->setPosition(math::Vec3f(posX, 0, 0));
                menus.push_back(menu);
            }
            void MenuBar::clear() {
                if (background != rect.getFillColor())
                    rect.setFillColor(background);
            }
            void MenuBar::draw(RenderTarget& target, RenderStates states) {
                target.draw(rect, states);
            }
            void MenuBar::pushEvent(sf::Event event) {
                getListener().pushEvent(event);
            }
            void MenuBar::checkSubWindowEvents() {
            }
        }
    }
}
