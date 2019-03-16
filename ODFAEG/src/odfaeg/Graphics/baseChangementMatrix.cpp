#include "../../../include/odfaeg/Graphics/baseChangementMatrix.h"
namespace odfaeg {
    namespace graphic {
        void BaseChangementMatrix::set2DIsoMatrix() {
            matrix2f.m11 = 0.5f;
            matrix2f.m12 = -1.f;
            matrix2f.m21 = 0.25f;
            matrix2f.m22 = 0.5f;
            invMatrix2f = matrix2f.inverse();
        }
        math::Vec2f BaseChangementMatrix::changeOfBase(math::Vec2f v) {
            math::Vec2f p = matrix2f * v;
            p = math::Vec2f ((int) p.x, (int) p.y);
            return p;
        }
        math::Vec2f BaseChangementMatrix::unchangeOfBase(math::Vec2f v) {
            math::Vec2f p = invMatrix2f * v;
            p = math::Vec2f ((int) p.x, (int) p.y);
            return p;
        }
        math::Matrix2f BaseChangementMatrix::getMatrix() {
            return matrix2f;
        }
    }
}
