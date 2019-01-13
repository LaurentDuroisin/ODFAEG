#include "../../../../include/odfaeg/Graphics/GUI/textArea.hpp"
namespace odfaeg {
    namespace graphic {
        namespace gui {
            TextArea::TextArea(math::Vec3f position, math::Vec3f size, const Font* font, std::string t, RenderWindow& rw) :
                LightComponent(rw, position, size, math::Vec3f(0, 0, 0)) {
                tmp_text = t;
                background = sf::Color::White;
                text.setFont(*font);
                text.setString(t);
                text.setColor(sf::Color::Black);
                text.setPosition(position);
                text.setSize(size);
                rect = RectangleShape(size);
                /*rect.setOutlineThickness(5.f);
                rect.setOutlineColor(sf::Color::Black);*/
                core::Action a2 (core::Action::MOUSE_BUTTON_PRESSED_ONCE, sf::Mouse::Left);
                core::Command cmd2(a2, core::FastDelegate<bool>(&TextArea::isMouseInTextArea, this), core::FastDelegate<void>(&TextArea::gaignedFocus, this));
                core::Command cmd3(a2, core::FastDelegate<bool>(&TextArea::isMouseOutTextArea, this), core::FastDelegate<void>(&TextArea::lostFocus, this));
                getListener().connect("CGFOCUS", cmd2);
                getListener().connect("CLFOCUS", cmd3);
                core::Action a3 (core::Action::MOUSE_BUTTON_PRESSED_ONCE, sf::Mouse::Left);
                core::Command cmd4(a3, core::FastDelegate<bool>(&TextArea::isMouseInTextArea, this), core::FastDelegate<void>(&TextArea::setCursorPos, this));
                getListener().connect("CMOUSECLICKED", cmd4);
                core::Action a4 (core::Action::TEXT_ENTERED);
                core::Command cmd5(a4, core::FastDelegate<bool>(&TextArea::hasFocus, this), core::FastDelegate<void>(&TextArea::onTextEntered, this, 'a'));
                getListener().connect("CTEXTENTERED", cmd5);
                currentIndex = tmp_text.size() - 1;
                sf::Vector2f pos = text.findCharacterPos(currentIndex);
                cursorPos = math::Vec3f(pos.x, pos.y, 0);
                setSize(text.getSize());
                haveFocus = textChanged = false;
            }
            void TextArea::onEventPushed(sf::Event event, RenderWindow& window) {
                if (&window == &getWindow()) {
                    getListener().pushEvent(event);
                }
            }
            bool TextArea::hasFocus() {
                return haveFocus;
            }
            void TextArea::setCursorPos() {
                math::Vec2f mousePos = math::Vec2f(sf::Mouse::getPosition(getWindow()).x, sf::Mouse::getPosition(getWindow()).y);
                bool found = false;
                for (unsigned int i = 0; i <= tmp_text.length() && !found; i++) {
                    sf::Vector2f pos = text.findCharacterPos(i);
                    if (pos.x > mousePos.x && pos.y > mousePos.y - text.getCharacterSize()) {
                        cursorPos = math::Vec3f(pos.x, pos.y, 0);
                        currentIndex = i;
                        found = true;
                    }
                }
            }
            void TextArea::gaignedFocus() {
                haveFocus = true;
                onGaignedFocus();
            }
            void TextArea::lostFocus() {
                haveFocus = false;
                onLostFocus();
            }
            void TextArea::onGaignedFocus() {
            }
            void TextArea::onLostFocus() {
            }
            void TextArea::clear() {
                if (background != rect.getFillColor())
                    rect.setFillColor(background);
            }
            void TextArea::setTextSize(unsigned int size) {
                text.setCharacterSize(size);
                setSize(text.getSize());
            }
            void TextArea::setTextColor(sf::Color color) {
                text.setColor(color);
            }
            std::string TextArea::getText() {
                return tmp_text;
            }
            void TextArea::onDraw(RenderTarget& target, RenderStates states) {
                VertexArray va(sf::Lines);
                va.append(Vertex(sf::Vector3f(cursorPos.x, cursorPos.y, 0), sf::Color::Black));
                va.append(Vertex(sf::Vector3f(cursorPos.x, cursorPos.y + text.getCharacterSize(), 0), sf::Color::Black));
                rect.setPosition(getPosition());
                text.setPosition(getPosition());
                rect.setSize(getSize());
                text.setSize(getSize());
                target.draw(rect);
                target.draw(text);
                target.draw(va);
            }
            bool TextArea::isMouseInTextArea() {
                physic::BoundingBox bb = getGlobalBounds();
                math::Vec2f mousePos = math::Vec2f(sf::Mouse::getPosition(getWindow()).x, sf::Mouse::getPosition(getWindow()).y);
                if (bb.isPointInside(mousePos)) {
                    return true;
                }
                return false;
            }
            bool TextArea::isMouseOutTextArea() {
                physic::BoundingBox bb = getGlobalBounds();
                math::Vec2f mousePos = math::Vec2f(sf::Mouse::getPosition(getWindow()).x, sf::Mouse::getPosition(getWindow()).y);
                if (bb.isPointInside(mousePos)) {
                    return false;
                }
                return true;
            }
            void TextArea::onUpdate(RenderWindow* window, sf::Event& event) {
                if (window == &getWindow() && event.type == sf::Event::TextEntered) {
                    getListener().setCommandSlotParams("CTEXTENTERED", this, static_cast<char>(event.text.unicode));
                }
            }
            void TextArea::onTextEntered(char caracter) {
                if (tmp_text.length() > 0 && caracter == 8) {
                    tmp_text.erase(currentIndex-1, 1);
                    currentIndex--;
                }
                else if (caracter != 8) {
                    tmp_text.insert(currentIndex, 1, caracter);
                    currentIndex++;
                }
                setText(tmp_text);
                sf::Vector2f pos = text.findCharacterPos(currentIndex);
                cursorPos = math::Vec3f(pos.x, pos.y, 0);
            }
            void TextArea::setText(std::string text) {
                tmp_text = text;
                this->text.setString(tmp_text);
                textChanged = true;
                //setSize(this->text.getSize());
            }
            bool TextArea::isTextChanged() {
                bool b = textChanged;
                textChanged = false;
                return b;
            }
        }
    }
}
