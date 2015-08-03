#ifndef ODFAEG_MENU_ITEM
#define ODFAEG_MENU_ITEM
#include "../rectangleShape.h"
#include "../renderWindow.h"
#include "../lightComponent.h"
#include "../../Math/vec4.h"
#include "../text.h"
namespace odfaeg {
    namespace graphic {
        namespace gui {
            class MenuItem : public LightComponent {
            public :
                MenuItem(RenderWindow& rw, Font* font, sf::String text);
                void clear();
                void setPosition (math::Vec3f position);
                void draw(RenderTarget& target, RenderStates states=RenderStates::Default);
            private :
                RenderWindow& window;
                RectangleShape shape;
                Text text;
                sf::Color background;
            };
        }
    }
}
#endif // ODFAEG_MENU_ITEM
