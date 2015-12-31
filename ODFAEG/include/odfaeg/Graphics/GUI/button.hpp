#ifndef ODFAEG_BUTTON_HPP
#define ODFAEG_BUTTON_HPP
#include "../lightComponent.h"
#include "../renderWindow.h"
#include "../rectangleShape.h"
#include "../text.h"
#include "../font.h"
#include "actionListener.hpp"
namespace odfaeg {
    namespace graphic {
        namespace gui {
            class Button : public LightComponent {
                public :
                    Button (math::Vec3f position, math::Vec3f size, const Font* font, std::string t, RenderWindow& rw);
                    void clear();
                    void draw(RenderTarget& target, RenderStates states=RenderStates::Default);
                    void setTextSize(unsigned int size);
                    void setTextColor(sf::Color color);
                    std::string getText();
                    void addActionListener(ActionListener* al);
                    bool isMouseInButton();
                    void pushEvent(sf::Event event);
                    void checkSubWindowEvents();
                    void onUpdate(RenderWindow* window, sf::Event& event);
                private :
                    Text text;
                    RectangleShape rect;
                    sf::Color background;
                    math::Vec3f mousePos;
            };
        }
    }
}
#endif // ODFAEG_BUTTON_HPP
