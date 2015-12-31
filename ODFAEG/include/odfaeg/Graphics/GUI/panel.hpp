#ifndef ODFAEG_PANEL_HPP
#define ODFAEG_PANEL_HPP
#include "../lightComponent.h"
#include "../renderWindow.h"
#include "../rectangleShape.h"
namespace odfaeg {
    namespace graphic {
        namespace gui {
            class Panel : public LightComponent {
            public :
                Panel(RenderWindow& window, math::Vec3f position, math::Vec3f size, LightComponent* parent = nullptr);
                void setBackgroundColor(sf::Color color);
                void clear();
                void draw(RenderTarget& target, RenderStates states);
                void addChild(LightComponent* child);
                void addDrawable(Drawable* drawable);
                std::vector<LightComponent*> getChildren();
                void removeAll();
                void pushEvent(sf::Event event);
                void checkSubWindowEvents();
                bool isOnXScroll();
                bool isOnYScroll();
                void moveXItems();
                void moveYItems();
            private :
                bool scrollX, scrollY;
                RectangleShape rect;
                RectangleShape vertScrollBar, horScrollBar, corner;
                math::Vec3f maxSize;
                int oldMouseX, oldMouseY, mouseDeltaX, mouseDeltaY;
                sf::Color background;
                std::vector<std::unique_ptr<LightComponent>> children;
                std::vector<Drawable*> drawables;
            };
        }
    }
}
#endif
