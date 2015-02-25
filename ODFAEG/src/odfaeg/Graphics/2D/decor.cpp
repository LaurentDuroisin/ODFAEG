#include "../../../../include/odfaeg/Graphics/2D/decor.h"
namespace odfaeg {
    namespace graphic {
        namespace g2d {
            using namespace sf;
            Decor::Decor (Tile *t, Light *light, int height, Shadow::SHADOW_TYPE shadowType) : Model(t->getPosition(), t->getSize(), t->getSize() * 0.5f, "E_DECOR") {
                this->height = height;
                float sy = light->getHeight() / (light->getHeight() * 0.75f);
                shadowCenter = math::Vec3f (0, 0, 0);
                shadowScale = math::Vec3f(1.f, -sy, 1.f);
                shadowRotationAngle = 0;
                shadowRotationAxis = math::Vec3f::zAxis;
                addChild(t);
                Shadow* shadow;
                if (shadowType == Shadow::SHADOW_TYPE::SHADOW_TILE)
                    shadow = new ShadowTile(*light,this);
                else
                    shadow = new ShadowWall(*light, this);
                addChild(shadow);
                shadow->setParent(this);
                t->setParent(this);
                this->shadowType = shadowType;
                shadowOrigin = math::Vec3f(t->getPosition().x, t->getPosition().y, 0) + getSize();
            }
            void Decor::createShadow(Light& light) {
                Shadow* shadow;
                if (getChildren().size() < 2) {
                    if (shadowType == Shadow::SHADOW_WALL)
                        shadow = new ShadowWall(light, this);
                    else
                        shadow = new ShadowTile(light, this);
                    shadow->setParent(this);
                    addChild(shadow);
                } else {
                    static_cast<Shadow*>(getChildren()[1])->createShadow(light, this);
                }
            }
            void Decor::setShadowCenter(math::Vec3f shadowCenter) {
                static_cast<Shadow*>(getChildren()[1])->changeCenter(shadowCenter);
                this->shadowCenter = shadowCenter;
            }
            math::Vec3f Decor::getShadowCenter() {
                return shadowCenter;
            }
            void Decor::setShadowScale(math::Vec3f shadowScale) {
                this->shadowScale = shadowScale;
            }
            math::Vec3f Decor::getShadowScale() {
                return shadowScale;
            }
            void Decor::setShadowRotation(float angle, math::Vec3f axis) {
                this->shadowRotationAngle = shadowRotationAngle;
                this->shadowRotationAxis = shadowRotationAxis;
            }
            math::Vec3f Decor::getShadowRotationAxis() {
                return shadowRotationAxis;
            }
            float Decor::getShadowRotationAngle() {
                return shadowRotationAngle;
            }
            void Decor::setShadowOrigin(math::Vec3f origin) {
                shadowOrigin = origin;
            }
            math::Vec3f Decor::getShadowOrigin() {
                return shadowOrigin;
            }
            void Decor::onMove(math::Vec3f &t) {
                Entity::onMove(t);
            }
            bool Decor::operator==(Entity &other) {
                if (other.getType() != "E_DECOR")
                    return false;
                Decor &decor = static_cast<Decor&> (other);
                return *getChildren()[0] == *decor.getChildren()[0];
            }
        }
    }
}
