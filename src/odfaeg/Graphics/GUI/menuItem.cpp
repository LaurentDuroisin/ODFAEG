#include "../../../../include/odfaeg/Graphics/GUI/menuItem.hpp"
namespace odfaeg {
    namespace graphic {
        namespace gui {
            MenuItem::MenuItem(RenderWindow& rw, Font* font, sf::String t) :
                LightComponent(math::Vec3f(0, 0, 0), math::Vec3f(0, 0, 0), math::Vec3f(0, 0, 0), false),
                window (rw) {
                    text.setFont(*font);
                    text.setString(t);
                    text.setColor(sf::Color::Black);
                    setSize(text.getSize());
                    setOrigin(text.getSize() * 0.5f);
                    shape = RectangleShape(getSize());
                    background = sf::Color(0.5f, 0.5f,0.5f);
                }
                void MenuItem::clear() {
                    shape.setFillColor(background);
                }
                void MenuItem::setPosition(math::Vec3f position) {
                    setPosition(position);
                    text.setPosition(position);
                    shape.setPosition(position);
                }
                void MenuItem::draw(RenderTarget& target, RenderStates states) {
                    target.draw(shape, states);
                    target.draw(text, states);
                }
        }
    }
}
