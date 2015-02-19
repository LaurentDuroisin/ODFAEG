#include "../../../../include/odfaeg/Graphics/GUI/textArea.hpp"
namespace odfaeg {
    namespace graphic {
        namespace gui {
            TextArea::TextArea(math::Vec3f position, math::Vec3f size, const Font* font, sf::String t, RenderWindow& rw) :
                rw(rw),
                LightComponent(position, size, size * 0.5f, false) {
                background = sf::Color::White;
                text.setFont(*font);
                text.setString(t);
                text.setColor(sf::Color::Black);
                text.setPosition(position);
                text.setSize(size);
                rect = RectangleShape(size);
                rect.setOutlineThickness(5.f);
                rect.setOutlineColor(sf::Color::Black);
                core::FastDelegate<void> slot(&TextArea::onTextEntered, this, '0');
                core::Action a (core::Action::TEXT_ENTERED);
                core::Command cmd(a, slot);
                getListener().connect("CTEXTENTERED", cmd);
            }
            void TextArea::clear() {
                rect.setFillColor(background);
            }
            void TextArea::setTextSize(unsigned int size) {
                text.setCharacterSize(size);
            }
            void TextArea::setTextColor(sf::Color color) {
                text.setColor(color);
            }
            std::string TextArea::getText() {
                return tmp_text;
            }
            void TextArea::draw(RenderTarget& target, RenderStates states) {
                rect.setPosition(getPosition());
                target.draw(rect, states);
                target.draw(text, states);
            }
            bool TextArea::isMouseInTextArea() {
                physic::BoundingBox bb = getGlobalBounds();
                math::Vec2f mousePos = math::Vec2f(sf::Mouse::getPosition(rw).x, sf::Mouse::getPosition(rw).y);
                if (bb.isPointInside(mousePos)) {
                    return true;
                }
                return false;
            }
            void TextArea::addFocusListener(FocusListener* fl) {
                core::FastDelegate<bool> trigger(&TextArea::isMouseInTextArea, this);
                core::FastDelegate<void> slot(&FocusListener::gaignedFocus,fl, this);
                core::Action a (core::Action::EVENT_TYPE::MOUSE_BUTTON_PRESSED_ONCE, sf::Mouse::Left);
                core::Command cmd(a, trigger, slot);
                getListener().connect("CTEXTAREACLICKED", cmd);
            }
            void TextArea::onUpdate(sf::Event& event) {
                if (event.type == sf::Event::TextEntered) {
                    getListener().setCommandSlotParams("CTEXTENTERED", this, static_cast<char>(event.text.unicode));
                }
            }
            void TextArea::onTextEntered(char caracter) {
                if (tmp_text.length() > 0 && caracter == 8)
                    tmp_text.erase(tmp_text.length() - 1, 1);
                else if (caracter != 8)
                    tmp_text.append(1, caracter);
                text.setString(sf::String(tmp_text.c_str()));
            }
        }
    }
}
