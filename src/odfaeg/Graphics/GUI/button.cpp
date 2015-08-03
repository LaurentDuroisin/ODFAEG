#include "../../../../include/odfaeg/Graphics/GUI/button.hpp"
namespace odfaeg {
    namespace graphic {
        namespace gui {
            Button::Button(math::Vec3f position, math::Vec3f size, const Font* font, sf::String t, RenderWindow& rw) :
                rw(rw),
                LightComponent(position, size, size * 0.5f,false) {
                background = sf::Color::White;
                text.setFont(*font);
                text.setString(t);
                text.setColor(sf::Color::Black);
                text.setPosition(position);
                text.setSize(size);
                rect = RectangleShape(size);
                rect.setOutlineThickness(5.f);
                rect.setOutlineColor(sf::Color::Red);
            }
            void Button::clear() {
                rect.setFillColor(background);
            }
            void Button::setTextSize(unsigned int size) {
                text.setCharacterSize(size);
            }
            void Button::setTextColor(sf::Color color) {
                text.setColor(color);
            }
            void Button::draw(RenderTarget& target, RenderStates states) {
                rect.setPosition(getPosition());
                target.draw(rect, states);
                target.draw(text, states);
            }
            std::string Button::getText() {
                return text.getString().toAnsiString();
            }
            bool Button::isMouseInButton() {
                physic::BoundingBox bb = getGlobalBounds();
                math::Vec2f mousePos = math::Vec2f(sf::Mouse::getPosition(rw).x, sf::Mouse::getPosition(rw).y);
                if (bb.isPointInside(mousePos)) {
                    return true;
                }
                return false;
            }
            void Button::addActionListener (ActionListener *al) {
                core::FastDelegate<bool> trigger(&Button::isMouseInButton, this);
                core::FastDelegate<void> slot(&ActionListener::actionPerformed,al, this);
                core::Action a (core::Action::EVENT_TYPE::MOUSE_BUTTON_PRESSED_ONCE, sf::Mouse::Left);
                core::Command cmd(a, trigger, slot);
                getListener().connect("CBUTTONCLICKED", cmd);
            }
            void Button::pushEvent (sf::Event event) {
                getListener().pushEvent(event);
            }
            void Button::checkSubWindowEvents() {

            }
        }
    }
}
