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
                Panel(RenderWindow& window, math::Vec3f position, math::Vec3f size, int priority = 0, LightComponent* parent = nullptr);
                void setBackgroundColor(sf::Color color);
                void clear();
                void onDraw(RenderTarget& target, RenderStates states);
                void drawOn(RenderTarget& target, RenderStates states);
                void addDrawable(Drawable* drawable);
                void onSizeRecomputed();
                void removeAll();
                bool isOnXScroll();
                bool isOnYScroll();
                void moveXItems();
                void moveYItems();
                void setBorderThickness(float thickness);
                void setBorderColor(sf::Color color);
                void updateScrolls();
            private :
                bool scrollX, scrollY;
                RectangleShape rect;
                RectangleShape vertScrollBar, horScrollBar, corner;
                math::Vec3f maxSize;
                int oldMouseX, oldMouseY, mouseDeltaX, mouseDeltaY;
                sf::Color background, border;
                std::vector<std::unique_ptr<LightComponent>> children;
                std::vector<Drawable*> drawables;
            };
        }
    }
}
#endif
