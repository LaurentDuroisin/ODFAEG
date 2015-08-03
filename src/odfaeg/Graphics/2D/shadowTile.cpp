#include "../../../../include/odfaeg/Graphics/2D/shadowTile.h"
#include "../../../../include/odfaeg/Graphics/2D/decor.h"
namespace odfaeg {
    namespace graphic {
        namespace g2d {
            using namespace sf;
            ShadowTile::ShadowTile (const Light &light, Model *model) : Shadow(light, model, "E_SHADOW_TILE") {
                shadow = nullptr;
                createShadow(light, model);
            }
            void ShadowTile::createShadow (const Light &light, Model *model) {
                if (shadow != nullptr)
                    delete shadow;
                math::Vec3f lightPos (const_cast<Light&> (light).getLightCenter().x, const_cast<Light&> (light).getLightCenter().y, 0);
                math::Vec3f n = math::Vec3f(0, 1.f, 0.f);
                int angle = math::Math::toDegrees(lightPos.getAngleBetween(n, lightPos.cross(n)));
                float sy;

                //La hauteur doit être comprise en 0 et 400.
                if (const_cast<Light&> (light).getHeight() == 0)
                    sy = 1.f;
                else if (const_cast<Light&> (light).getHeight() >= 500)
                    sy = 0.f;
                else
                    sy = 1.f / (100.f / (float) const_cast<Light&> (light).getHeight());
                shadow = new Sprite(*model->getChildren()[0]->getFaces()[0]->getMaterial().getTexture(), model->getChildren()[0]->getPosition(), model->getChildren()[0]->getSize(), model->getChildren()[0]->getFaces()[0]->getMaterial().getTexRect());
                shadow->setScale(math::Vec3f(-1.f, sy, 1.f));
                shadow->setRotation(angle);
                shadow->setColor(Color(0, 0, 0));
                shadow->setOrigin(model->getOrigin());
                center = model->getShadowCenter();
                int centerX = model->getSize().x / 100 * model->getShadowCenter().x;
                int centerY = model->getSize().y / 100 * model->getShadowCenter().y;
                math::Vec3f center (centerX, centerY, center.z);
                shadow->setPosition(math::Vec3f(model->getPosition().x + center.x, model->getPosition().y + center.y, model->getPosition().y + center.y, model->getPosition().y + center.y + model->getSize().y * 0.5f));
            }
            Drawable* ShadowTile::getShadow() {
                return shadow;
            }
            void ShadowTile::changeCenter(math::Vec3f shadowCenter) {
                this->center = shadowCenter;
                int centerX = getSize().x / 100 * shadowCenter.x;
                int centerY = getSize().y / 100 * shadowCenter.y;
                shadow->setPosition(math::Vec3f(getPosition().x + centerX, getPosition().y + centerY, getPosition().y + center.y + getSize().y * 0.5f));
            }
            void ShadowTile::onMove(math::Vec3f &t) {
                shadow->move(t);
            }
            void ShadowTile::onDraw(RenderTarget &target, RenderStates states) {
                target.draw(*shadow, states);
            }
            bool ShadowTile::operator== (Entity& other) {
                if (other.getType() != "E_SHADOW_TILE")
                    return false;
                ShadowTile& shadowTile = static_cast<ShadowTile&> (other);
                return shadow->getPosition() == shadowTile.shadow->getPosition();
            }
            ShadowTile::~ShadowTile () {
                delete shadow;
            }
        }
    }
}


