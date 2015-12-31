#include "../../../../include/odfaeg/Graphics/GUI/menu.hpp"
namespace odfaeg {
    namespace graphic {
        namespace gui {
            Menu::Menu(RenderWindow& rw, const Font* font, std::string t) :
                LightComponent(rw, math::Vec3f(0, 0, 0), math::Vec3f(t.length() * 10, 20, 0), math::Vec3f(0, 0, 0))
{
                text.setString(sf::String(t.c_str()));
                text.setCharacterSize(15);
                text.setFont(*font);
                text.setColor(sf::Color::Black);
                rect = RectangleShape(getSize());
                rect.setOutlineThickness(1);
                rect.setOutlineColor(sf::Color::Black);
                core::Action a (odfaeg::core::Action::MOUSE_BUTTON_PRESSED_ONCE, sf::Mouse::Left);
                core::Command menuSelected (a, core::FastDelegate<void>(&Menu::onClick, this));
                getListener().connect("MenuSelected", menuSelected);
                background = sf::Color(50, 50, 50);
            }
            void Menu::clear() {
                if (background != rect.getFillColor())
                    rect.setFillColor(background);
            }
            void Menu::addMenuItem(MenuItem* item) {
                unsigned int yPos = getSize().y;
                for (unsigned int i = 0; i < items.size(); i++) {
                    yPos += items[i]->getSize().y;
                }
                item->setPosition(math::Vec3f(getPosition().x, yPos, 0));
                items.push_back(item);
            }
            void Menu::setPosition(math::Vec3f position) {
                Transformable::setPosition(position);
                text.setPosition(position);
                rect.setPosition(position);
                unsigned int yPos = getSize().y;
                for (unsigned int i = 0; i < items.size(); i++) {
                    items[i]->setPosition(math::Vec3f(position.x, yPos, position.z));
                    yPos += items[i]->getSize().y;
                }
            }
            void Menu::draw(RenderTarget& target, RenderStates states) {
                target.draw(rect, states);
                target.draw(text, states);
            }
            void Menu::onClick() {
                if (isMouseOnMenu()) {
                    for (unsigned int i = 0; i < items.size(); i++) {
                        items[i]->setVisible(true);
                        items[i]->setEventContextActivated(true);
                    }
                } else {
                    for (unsigned int i = 0; i < items.size(); i++) {
                        items[i]->setVisible(false);
                        items[i]->setEventContextActivated(false);
                    }
                }
            }
            bool Menu::isMouseOnMenu() {
                physic::BoundingBox bb = getGlobalBounds();
                math::Vec2f mousePos = math::Vec2f(sf::Mouse::getPosition(getWindow()).x, sf::Mouse::getPosition(getWindow()).y);
                if (bb.isPointInside(mousePos)) {
                    return true;
                }
                return false;
            }
            void Menu::pushEvent(sf::Event event) {
                getListener().pushEvent(event);
            }
            void Menu::checkSubWindowEvents() {
            }
        }
    }
}
