#ifndef LABEL_HPP
#define LABEL_HPP
#include "../rectangleShape.h"
#include "../renderWindow.h"
#include "../lightComponent.h"
#include "../../Math/vec4.h"
#include "../text.h"
namespace odfaeg {
    namespace graphic {
        namespace gui {
            class Label : public LightComponent {
                public :
                Label (RenderWindow& window, math::Vec3f position, math::Vec3f size, const Font* font, std::string text);
                void clear();
                void draw (RenderTarget& target, RenderStates states = RenderStates::Default);
                void setBackgroundColor(sf::Color color);
                sf::Color getBackgroundColor();
                void setForegroundColor(sf::Color color);
                sf::Color getForegroundColor();
                void pushEvent(sf::Event event);
                void checkSubWindowEvents();
                void setText(std::string text);
                std::string getText();
                bool isMouseInside();
                void onUpdate(RenderWindow* window, sf::Event& event);
                private :
                math::Vec3f mousePos;
                RectangleShape rect;
                Text text;
                sf::Color background;
            };
        }
    }
}
#endif
