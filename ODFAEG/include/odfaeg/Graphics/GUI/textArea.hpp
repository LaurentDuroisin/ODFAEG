#ifndef ODFAEG_TEXT_AREA
#define ODFAEG_TEXT_AREA
#include "../lightComponent.h"
#include "../renderWindow.h"
#include "../rectangleShape.h"
#include "../text.h"
#include "../font.h"
#include "focusListener.hpp"
namespace odfaeg {
    namespace graphic {
        namespace gui {
            class TextArea : public LightComponent {
            public :
                TextArea(math::Vec3f position, math::Vec3f size, const Font* font, std::string s, RenderWindow& rw);
                void clear();
                void draw(RenderTarget& target, RenderStates states);
                void setTextSize(unsigned int size);
                void setTextColor(sf::Color color);
                void addFocusListener(FocusListener* fl);
                bool isMouseInTextArea();
                void onTextEntered(char caracter);
                void onUpdate(RenderWindow* window, sf::Event& event);
                void pushEvent(sf::Event event);
                void checkSubWindowEvents();
                std::string getText();
            private :
                std::string tmp_text;
                Text text;
                RectangleShape rect;
                sf::Color background;
            };
        }
    }
}
#endif // ODFAEG_TEXT_AREA
