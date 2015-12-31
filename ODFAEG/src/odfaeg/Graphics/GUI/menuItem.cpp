#include "../../../../include/odfaeg/Graphics/GUI/menuItem.hpp"
namespace odfaeg {
    namespace graphic {
        namespace gui {
            MenuItem::MenuItem(RenderWindow& rw, const Font* font, std::string t) :
                LightComponent(rw, math::Vec3f(0, 0, 0), math::Vec3f(t.length() * 10, 20, 0), math::Vec3f(0, 0, 0), 1)
                {
                    text.setFont(*font);
                    text.setCharacterSize(15);
                    text.setString(sf::String(t.c_str()));
                    text.setColor(sf::Color::Black);
                    rect = RectangleShape(getSize());
                    rect.setOutlineThickness(1);
                    rect.setOutlineColor(sf::Color::Black);
                    background = sf::Color(50, 50, 50);
                    setVisible(false);
                    setEventContextActivated(false);
                }
                void MenuItem::clear() {
                    if (background != rect.getFillColor())
                        rect.setFillColor(background);
                }
                void MenuItem::setPosition(math::Vec3f position) {
                    Transformable::setPosition(position);
                    text.setPosition(position);
                    rect.setPosition(position);
                }
                void MenuItem::draw(RenderTarget& target, RenderStates states) {
                    target.draw(rect, states);
                    target.draw(text, states);
                }
                void MenuItem::addMenuItemListener (MenuItemListener *mil) {
                    core::FastDelegate<bool> trigger(&MenuItem::isMouseOnMenu, this);
                    core::FastDelegate<void> slot(&MenuItemListener::actionPerformed,mil, this);
                    core::Action a (core::Action::EVENT_TYPE::MOUSE_BUTTON_PRESSED_ONCE, sf::Mouse::Left);
                    core::Command cmd(a, trigger, slot);
                    getListener().connect("CITEMCLICKED", cmd);
                }
                bool MenuItem::isMouseOnMenu() {
                    physic::BoundingBox bb = getGlobalBounds();
                    math::Vec2f mousePos = math::Vec2f(sf::Mouse::getPosition(getWindow()).x, sf::Mouse::getPosition(getWindow()).y);
                    if (bb.isPointInside(mousePos)) {
                        return true;
                    }
                    return false;
                }
                void MenuItem::pushEvent(sf::Event event) {
                    getListener().pushEvent(event);
                }
                void MenuItem::checkSubWindowEvents() {
                }
        }
    }
}
