#include "../../../include/odfaeg/Graphics/billBoard.h"
namespace odfaeg {
    namespace graphic {
        BillBoard::BillBoard(View view, Drawable& drawable)
        : Entity (math::Vec3f(0, 0, 0), math::Vec3f(view.getSize().x, view.getSize().y, 0), math::Vec3f(view.getSize().x * 0.5f, view.getSize().y * 0.5f, 0), "E_BILLBOARD"),
        view(view),
        drawable(drawable)
        {

        }
        View BillBoard::getView() {
            return view;
        }
        void BillBoard::setView(View view) {
            this->view = view;
        }
        void BillBoard::draw(RenderTarget& target, RenderStates states) {
            math::Vec3f forward = view.getPosition() - getCenter();
            forward = forward.normalize();
            float angle = math::Math::toDegrees(forward.getAngleBetween(view.getForward(), view.getUp()));
            math::Vec3f left = forward.cross(view.getUp()).normalize();
            math::Vec3f axis = left.cross(forward);
            tm.setRotation(axis, angle);
            tm.update();
            states.transform.combine(tm.getMatrix());
            target.draw(drawable, states);
        }
    }
}
