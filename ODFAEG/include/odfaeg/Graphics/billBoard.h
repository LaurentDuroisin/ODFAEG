#ifndef ODFAEG_BILLBOARD_HPP
#define ODFAEG_BILLBOARD_HPP
#include "entity.h"
#include "../Physics/particuleSystem.h"
#include "../Math/vec4.h"
#include "renderTexture.h"
#include "tile.h"
namespace odfaeg {
    namespace graphic {
        class BillBoard : public Entity {
        public :
            BillBoard(View view, Drawable& drawable);
            void draw(RenderTarget& target, RenderStates states);
            View getView();
            bool operator== (Entity& other) {
                if (other.getType() != "E_BILLBOARD")
                    return false;
                BillBoard& bb = static_cast<BillBoard&>(other);
                if (!(tm == bb.tm))
                    return false;
                return true;
            }
            bool isAnimated() const {
                return false;
            }
            bool isModel() const {
                return false;
            }
            bool selectable() const {
                return false;
            }
            bool isLight() const {
                return false;
            }
            bool isShadow() const {
                return false;
            }
            bool isLeaf() const {
                return true;
            }
            void setView (View view);
        private :
            View view;
            TransformMatrix tm;
            Drawable& drawable;
        };
    }
}
#endif // ODFAEG_BILLBOARD_HPP
