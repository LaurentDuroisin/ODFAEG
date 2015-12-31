#include "../../../../include/odfaeg/Graphics/GUI/label.hpp"
namespace odfaeg {
    namespace graphic {
        namespace gui {
            Label::Label (RenderWindow& window, math::Vec3f position, math::Vec3f size, const Font* font, std::string t) :
            LightComponent (window, position, size, size * 0.5f) {
                text.setFont(*font);
                text.setString(t);
                text.setSize(size);
                unsigned int maxSize = std::max(size.x, size.y);
                unsigned int nbChars = text.getString().length();
                unsigned int char_size;
                if (nbChars != 0)
                    char_size = maxSize / nbChars;
                while (char_size * nbChars > size.x || char_size > size.y) {
                    char_size--;
                }
                rect = RectangleShape (size);
                rect.setPosition(position);
                background = sf::Color::Black;
                rect.setFillColor(background);
            }
            void Label::setForegroundColor(sf::Color color) {
                text.setColor(color);
            }
            sf::Color Label::getForegroundColor() {
                return text.getColor();
            }
            void Label::setBackgroundColor(sf::Color color) {
                this->background = color;
            }
            sf::Color Label::getBackgroundColor() {
                return background;
            }
            void Label::clear() {
                if (background != rect.getFillColor()) {
                    rect.setFillColor(background);
                }
            }
            void Label::draw(RenderTarget& target, RenderStates states) {
                states.transform = getTransform();
                target.draw(rect, states);
                target.draw(text, states);
            }
            void Label::setText(std::string t) {
                text.setString(t);
                unsigned int maxSize = std::max(getSize().x, getSize().y);
                unsigned int minSize = std::min(getSize().x, getSize().y);
                float s = (float) maxSize / (float) minSize;
                math::Vec3f scale;
                if (maxSize == getSize().x)
                    scale = math::Vec3f(1.f, s, 1.f);
                else
                    scale = math::Vec3f(s, 1.f, 1.f);
                text.setScale(scale);
                unsigned int nbChars = text.getString().length();
                unsigned int char_size;
                if (nbChars != 0)
                    char_size = maxSize / nbChars;
                while (char_size * nbChars > getSize().x || char_size > getSize().y)
                    char_size--;
                text.setCharacterSize(char_size);
            }
            std::string Label::getText() {
                return text.getString();
            }
            bool Label::isMouseInside() {
                physic::BoundingBox bb(getPosition().x, getPosition().y, 0, getSize().x, getSize().y, getSize().z);
                return bb.isPointInside(mousePos);
            }
            void Label::onUpdate(RenderWindow* window, sf::Event& event) {
                if (&getWindow() == window) {
                    if (event.type == sf::Event::MouseButtonPressed) {
                        mousePos = math::Vec3f(event.mouseButton.x, event.mouseButton.y, 0);
                    }
                }
            }
            void Label::pushEvent (sf::Event event) {
                getListener().pushEvent(event);
            }
            void Label::checkSubWindowEvents() {
            }
        }
    }
}
