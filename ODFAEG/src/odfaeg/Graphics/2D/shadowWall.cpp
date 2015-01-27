#include "../../../../include/odfaeg/Graphics/2D/shadowWall.h"
#include "../../../../include/odfaeg/Graphics/2D/wall.h"
namespace odfaeg {
    namespace graphic {
        namespace g2d {
            using namespace sf;
            using namespace std;
            ShadowWall::ShadowWall (const Light &light, Model *model) : Shadow (light, model, "E_SHADOW_WALL") {
                shadow = nullptr;
                createShadow (light, model);
            }
            void ShadowWall::createShadow (const Light &light, Model *model) {
                if (shadow != nullptr)
                    delete shadow;

                math::Vec3f lightPos (const_cast<Light&> (light).getLightCenter().x, const_cast<Light&> (light).getLightCenter().y, 0);
                math::Vec3f v = lightPos.normalize() * model->getHeight();

                int lightHeight = const_cast<Light&> (light).getHeight();
                float scale;
                if (lightHeight <= 0)
                    scale = 1.f;
                else if (lightHeight >= 200)
                    scale = 0.f;
                else
                    scale = 1.f /  (100.f / (float) const_cast<Light&> (light).getHeight());

                shadow = new ConvexShape (model->getPoints().size() * 2);
                vector<math::Vec3f> points;

                math::Vec3f positionShadow (model->getPoints()[0]->x, model->getPoints()[0]->y, 0);

                for (unsigned int i = 0; i < model->getPoints().size(); i++) {
                    math::Vec3f point (*model->getPoints()[i] - positionShadow);
                    shadow->setPoint(i, Vector3f((int) point.x, (int) point.y, 0));
                    points.push_back(point);
                }

                shadow->setFillColor(Color(0, 0, 0));
                long int i = static_cast<long int>(model->getPoints().size() - 1);
                for (unsigned int j = model->getPoints().size(); i >=0 ; j++, i--) {
                    math::Vec3f projPoint (*model->getPoints()[i] - positionShadow + (v * scale));
                    shadow->setPoint(j, Vector3f ((int) projPoint.x , (int) projPoint.y, 0));
                    points.push_back(projPoint);
                }

                math::Vec3f shadowOrigin = math::Computer::getMoy(points);
                center = model->getShadowCenter();
                shadow->setScale(math::Vec3f(1.f, scale, 1.f));
                shadow->setFillColor(Color(0, 0, 0));
                shadow->setPosition(math::Vec3f(positionShadow.x + center.x, positionShadow.y + center.y, positionShadow.y + center.y + shadow->getSize().y * 0.5f));
                shadow->setOrigin(math::Vec3f(shadowOrigin.x, shadowOrigin.y, 0));
                points.clear();

            }
            void ShadowWall::changeCenter(math::Vec3f shadowCenter) {
                this->center = shadowCenter;
                int centerX = getSize().x / 100 * shadowCenter.x;
                int centerY = getSize().y / 100 * shadowCenter.y;
                shadow->setPosition(math::Vec3f(getPosition().x + centerX, getPosition().y + centerY, getPosition().y + centerY + getSize().y * 0.5f));
            }
            void ShadowWall::onMove(math::Vec3f& t) {
                shadow->move(t);
            }
            void ShadowWall::onDraw(RenderTarget &target, RenderStates states) {
                 target.draw(*shadow, states);
            }
            bool ShadowWall::operator== (Entity& other) {
                if (other.getType() != "E_SHADOW_WALL")
                    return false;
                ShadowWall &shadowWall = static_cast<ShadowWall&> (other);
                if (shadow->getPointCount() != shadowWall.shadow->getPointCount())
                    return false;
                for (unsigned int i = 0; i < shadow->getPointCount(); i++)
                    if (shadow->getPoint(i) != shadowWall.shadow->getPoint(i))
                        return false;
                return shadow->getPosition().x == shadowWall.shadow->getPosition().x;
            }
            Drawable* ShadowWall::getShadow () {
                return shadow;
            }

            ShadowWall::~ShadowWall () {
                delete shadow;
            }
        }
    }
}
