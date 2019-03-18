#include "../../../../include/odfaeg/Graphics/GUI/progressBar.hpp"
namespace odfaeg {
    namespace graphic {
        namespace gui {
            ProgressBar::ProgressBar(RenderWindow& rw, math::Vec3f position, math::Vec3f size, const Font& font, unsigned int charSize) :
            LightComponent(rw, position, size, size*0.5f) {
                bar = RectangleShape(size);
                bar.setPosition(position);
                bar.setFillColor(sf::Color::Red);
                this->value = 0;
                text.setFont(font);
                text.setCharacterSize(charSize);
                text.setSize(size);
                text.setPosition(position);
                text.setString(core::conversionIntString(value)+"/"+core::conversionIntString(maxi));
            }
            void ProgressBar::clear() {
                bar.setFillColor(sf::Color::Red);
            }
            void ProgressBar::onDraw(RenderTarget& target, RenderStates states) {
                target.draw(bar);
                target.draw(text);
            }
            void ProgressBar::setMaximum(int maxi) {
                text.setString(core::conversionIntString(value)+"/"+core::conversionIntString(maxi));
                this->maxi = maxi;
            }
            void ProgressBar::setMinimum(int mini) {
                this->mini = mini;
            }
            void ProgressBar::setValue(int value) {
                int sizeX = getSize().x / (maxi - mini) * value;
                if (LightComponent::getName() == "HPBAR")
                    std::cout<<"size :  "<<getSize().x<<" maxi : "<<maxi<<" mini : "<<mini<<" value : "<<value<<" size X : "<<sizeX<<std::endl;
                bar.setSize(math::Vec3f(sizeX, bar.getSize().y, bar.getSize().z));
                this->value = value;
                text.setString(core::conversionIntString(value)+"/"+core::conversionIntString(maxi));
                if (LightComponent::getName() == "HPBAR")
                    bar.setName("HPBAR");
            }
            int ProgressBar::getValue() {
                return value;
            }
        }
    }
}
