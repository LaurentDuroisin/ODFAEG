#include "../../../../include/odfaeg/Graphics/GUI/icon.hpp"
namespace odfaeg {
    namespace graphic {
        namespace gui {
            Icon::Icon(RenderWindow& window, math::Vec3f position, math::Vec3f size, Sprite icon) : LightComponent (window, position, size, size * 0.5f), icon(icon) {
            }
            void Icon::onDraw (RenderTarget& target, RenderStates states) {
                target.draw(icon, states);
            }
        }
    }
}
