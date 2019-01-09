#include "../../../../include/odfaeg/Graphics/GUI/progressBar.hpp"
namespace odfaeg {
    namespace graphic {
        namespace gui {
            ProgressBar::ProgressBar(RenderWindow& rw, math::Vec3f position, math::Vec3f size) :
            LightComponent(rw, position, size, size*0.5f) {
                border = RectangleShape(size);
                border.setPosition(position);
                border.setFillColor(sf::Color(0, 0, 0, 0));
                border.setOutlineThickness(5.f);
                border.setOutlineColor(sf::Color(0, 0, 0, 255));
                bar = RectangleShape(size);
                bar.setPosition(position);
                bar.setFillColor(sf::Color::Red);
                this->value = 0;
            }
            void ProgressBar::clear() {
                bar.setFillColor(sf::Color::Red);
            }
            void ProgressBar::onDraw(RenderTarget& target, RenderStates states) {
                target.draw(border);
                target.draw(bar);
            }
            void ProgressBar::setMaximum(int maxi) {
                this->maxi = maxi;
            }
            void ProgressBar::setMinimum(int mini) {
                this->mini = mini;
            }
            void ProgressBar::setValue(int value) {
                int sizeX = getSize().x / (maxi - mini) * value;
                bar.setSize(math::Vec3f(sizeX, bar.getSize().y, bar.getSize().z));
                this->value = value;
            }
            int ProgressBar::getValue() {
                return value;
            }
        }
    }
}
