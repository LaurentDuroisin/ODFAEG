#ifndef PROGRESS_BAR
#define PROGRESS_BAR
#include "../rectangleShape.h"
#include "../renderWindow.h"
#include "../lightComponent.h"
#include "../../Math/vec4.h"
namespace odfaeg {
    namespace graphic {
        namespace gui {
            class ProgressBar : public LightComponent {
            public :
            ProgressBar(RenderWindow& window, math::Vec3f position, math::Vec3f size);
            void onDraw (RenderTarget& target, RenderStates states = RenderStates::Default);
            void setMaximum (int maxi);
            void setMinimum (int mini);
            void setValue (int value);
            void clear();
            int getValue();
            private :
            RectangleShape border, bar;
            int maxi, mini, value;
            };
        }
    }
}
#endif // PROGRESS_BAR
