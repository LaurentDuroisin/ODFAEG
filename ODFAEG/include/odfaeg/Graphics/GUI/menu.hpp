#ifndef MENU_HPP
#define MENU_HPP
#include "menuItem.hpp"
namespace odfaeg {
    namespace graphic {
        namespace gui {
            class Menu : public LightComponent {
            public :
                Menu(RenderWindow& rw, const Font* font, std::string text);
                void addMenuItem(MenuItem* item);
                void clear();
                void setPosition (math::Vec3f position);
                void draw(RenderTarget& target, RenderStates states=RenderStates::Default);
                void onClick();
                bool isMouseOnMenu();
                void pushEvent(sf::Event event);
                void checkSubWindowEvents();
            private :
                RenderWindow& window;
                RectangleShape rect;
                Text text;
                sf::Color background;
                std::vector<MenuItem*> items;
            };
        }
    }
}
#endif
