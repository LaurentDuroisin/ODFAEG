#include "../../../../include/odfaeg/Graphics/2D/wall.h"
namespace odfaeg {
    namespace graphic {
        namespace g2d {

            Wall::Wall (int imageId, int height, Tile *tile, Light *light, Shadow::SHADOW_TYPE shadowType) : Model (tile->getPosition(),tile->getSize(), tile->getOrigin(), "E_WALL")  {
                this->height = height;
                math::Vec2f size (tile->getSize().x, tile->getSize().y);
                math::Vec2f position (tile->getPosition().x, tile->getPosition().y);
                type = imageId;
                this->shadowType = shadowType;
                addChild(tile);
                tile->setParent(this);
                float sy = light->getHeight() / (light->getHeight() * 0.75f);
                math::Matrix3f chBase (1.f, -1.f, 0.f, 0.5f, 0.5f, 0.f, 0.f, 0.f, 1.f);
                if (imageId == 0) {
                    math::Vec3f p1 (position.x, position.y, 0);
                    math::Vec3f d = chBase * math::Vec3f(size.x, 0, 0);
                    math::Vec3f p2 (position.x + size.x, position.y + d.y, 0);
                    math::Ray *r = new math::Ray (p1, p2);
                    addSegment(r);
                    math::Vec3f* bottom1 = new math::Vec3f (p1.x, p1.y + (int) height / 2, 0);
                    math::Vec3f* bottom2 = new math::Vec3f (p2.x, p2.y + (int) height / 2, 0);
                    addShadowPoint(bottom1);
                    addShadowPoint(bottom2);
                } else if (imageId == 1) {
                    math::Vec3f d = chBase * math::Vec3f (size.x, 0, 0);
                    math::Vec3f p1 = math::Vec3f (position.x, position.y + d.y, 0);
                    math::Vec3f p2 = math::Vec3f (position.x + size.x, position.y, 0);
                    math::Ray *r = new math::Ray (p1, p2);
                    addSegment(r);
                    math::Vec3f* bottom1 = new math::Vec3f (p1.x, p1.y + (int) height / 2, 0);
                    math::Vec3f* bottom2 = new math::Vec3f (p2.x, p2.y + (int) height / 2, 0);
                    addShadowPoint(bottom1);
                    addShadowPoint(bottom2);
                } else if (imageId == 2) {
                    math::Vec3f d = chBase * math::Vec3f (size.x / 2, 0, 0);
                    math::Vec3f p1 = math::Vec3f (position.x, position.y, 0);
                    math::Vec3f p2 = math::Vec3f (position.x + size.x / 2, position.y + d.y, 0);
                    math::Ray *r1 = new math::Ray (p1, p2);
                    math::Vec3f* bottom1 = new math::Vec3f (p1.x, p1.y + (int) height / 2, 0);
                    math::Vec3f* bottom2 = new math::Vec3f (p2.x, p2.y + (int) height / 2, 0);
                    addShadowPoint(bottom1);
                    addShadowPoint(bottom2);
                    addSegment(r1);
                    p1 = math::Vec3f (position.x + size.x / 2, position.y + d.y, 0);
                    p2 = math::Vec3f (position.x + size.x, position.y, 0);
                    math::Ray *r2 = new math::Ray (p1, p2);
                    addSegment(r2);
                    bottom1 = new math::Vec3f (p1.x + 10, p1.y + (int) height / 2, 0);
                    bottom2 = new math::Vec3f (p2.x, p2.y + (int) height / 2, 0);
                    addShadowPoint(bottom1);
                    addShadowPoint(bottom2);
                } else if (imageId == 3) {
                    math::Vec3f d = chBase * math::Vec3f (size.x / 2, 0, 0);
                    math::Vec3f p1 = math::Vec3f (position.x, position.y + d.y + 20, 0);
                    math::Vec3f p2 = math::Vec3f (position.x + size.x / 2, position.y + 20, 0);
                    math::Ray *r1 = new math::Ray (p1, p2);
                    math::Vec3f* bottom1 = new math::Vec3f (p1.x, p1.y + (int) height / 2, 0);
                    math::Vec3f* bottom2 = new math::Vec3f (p2.x - 10, p2.y + (int) height / 2, 0);
                    addShadowPoint(bottom1);
                    addShadowPoint(bottom2);
                    addSegment(r1);
                    p1 = math::Vec3f (position.x + size.x / 2, position.y + 20, 0);
                    p2 = math::Vec3f (position.x + size.x, position.y + d.y + 20, 0);
                    math::Ray *r2 = new math::Ray (p1, p2);
                    addSegment(r2);
                    bottom1 = new math::Vec3f (p1.x + 10, p1.y + (int) height / 2, 0);
                    bottom2 = new math::Vec3f (p2.x, p2.y + (int) height / 2, 0);
                    addShadowPoint(bottom1);
                    addShadowPoint(bottom2);
                } else if (imageId == 4) {
                    math::Vec3f d = chBase * math::Vec3f (size.x / 2, 0, 0);
                    math::Vec3f p1 = math::Vec3f (position.x - 10, position.y, 0);
                    math::Vec3f p2 = math::Vec3f (position.x + size.x / 4 * 3 + 10, position.y + d.y + 10, 0);
                    math::Ray *r1 = new math::Ray (p1, p2);
                    addSegment(r1);
                    p1 = math::Vec3f (position.x + size.x / 4 * 3 + 10, position.y + d.y - 10, 0);
                    p2 = math::Vec3f (position.x - 10, position.y + size.y / 2, 0);
                    math::Ray *r2 = new math::Ray (p1, p2);
                    addSegment(r2);
                    math::Vec3f *bottom1 = new math::Vec3f (position.x + size.x / 2, p1.y + (int) height / 2, 0);
                    math::Vec3f *bottom2 = new math::Vec3f (p2.x, p2.y + (int) height / 2, 0);
                    addShadowPoint(bottom1);
                    addShadowPoint(bottom2);
                } else if (imageId == 5) {
                    math::Vec3f d = chBase * math::Vec3f (size.x / 2, 0, 0);
                    math::Vec3f p1 = math::Vec3f (position.x + size.x / 4 * 3 + 10, position.y + size.y / 3 + 10, 0);
                    math::Vec3f p2 = math::Vec3f (position.x , position.y + d.y - 10, 0);
                    math::Ray *r1 = new math::Ray (p1, p2);
                    addSegment (r1);
                    math::Vec3f *bottom1 = new math::Vec3f (p1.x , p1.y + (int) height / 2, 0);
                    math::Vec3f *bottom2 = new math::Vec3f (position.x + size.x / 4, p2.y + (int) height / 2, 0);
                    addShadowPoint(bottom1);
                    addShadowPoint(bottom2);
                    p1 = math::Vec3f (position.x, position.y + d.y + 10, 0);
                    p2 = math::Vec3f (position.x + size.x / 4 * 3 + 10, position.y + 10, 0);
                    math::Ray *r2 = new math::Ray (p1, p2);
                    addSegment(r2);
                }
                shadowScale = math::Vec3f(1.f, sy, sy);

                shadowCenter = math::Vec3f(0, 0, 0);
                shadowRotationAngle = 0;
                shadowRotationAxis = math::Vec3f::zAxis;
                Shadow* shadow;
                if (shadowType == Shadow::SHADOW_WALL)
                    shadow = new ShadowWall(*light, this);
                else
                    shadow = new ShadowTile(*light, this);
                shadow->setParent(this);
                addChild(shadow);
                shadowOrigin = math::Vec3f(tile->getPosition().x, tile->getPosition().y, 0);
            }
            void Wall::createShadow(Light& light) {
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
            bool Wall::isLeaf() const {
                return false;
            }
            int Wall::getHeight() {
                return height;
            }



            /*void Wall::onRotate(float angle) {
                TransformMatrix tm;
                tm.setRotation(angle - getRotation());
                tm.setOrigin(getCenter().x, getCenter().y);
                for (unsigned int i = 0; i < segments.size(); i++) {
                    math::Vec2f orig = segments[i]->getOrig();
                    math::Vec2f ext = segments[i]->getExt();
                    orig = tm.transform(orig);
                    ext = tm.transform(ext);
                    segments[i]->setOrig(orig);
                    segments[i]->setExt(ext);
                }
                for (unsigned int i = 0; i < bottomPoints.size(); i++) {
                    *bottomPoints[i] = tm.transform(*bottomPoints[i]);
                }
                shadow->setRotation(angle);
            }
            void Wall::onScale(math::Vec3f s) {
                TransformMatrix tm;
                tm.setScale(getScale().x / s.x, getScale().y / s.y);
                tm.setOrigin(getCenter().x, getCenter().y);
                for (unsigned int i = 0; i < segments.size(); i++) {
                    math::Vec2f orig = segments[i]->getOrig();
                    math::Vec2f ext = segments[i]->getExt();
                    orig = tm.transform(orig);
                    ext = tm.transform(ext);
                    segments[i]->setOrig(orig);
                    segments[i]->setExt(ext);
                }
                for (unsigned int i = 0; i < bottomPoints.size(); i++) {
                    *bottomPoints[i] = tm.transform(*bottomPoints[i]);
                }
                shadow->setScale(s);
            }*/
            void Wall::onMove(math::Vec3f& t) {
                Entity::onMove(t);
                TransformMatrix tm;
                tm.setTranslation(math::Vec3f(t.x - getLocalPos().x, t.y - getLocalPos().y, 0));
                tm.setOrigin(math::Vec3f(getCenter().x, getCenter().y, 0));
                for (unsigned int i = 0; i < getSegments().size(); i++) {
                    math::Vec3f orig = getSegments()[i]->getOrig();
                    math::Vec3f ext = getSegments()[i]->getExt();
                    orig = tm.transform(orig);
                    ext = tm.transform(ext);
                    getSegments()[i]->setOrig(orig);
                    getSegments()[i]->setExt(ext);
                }
                for (unsigned int i = 0; i < getPoints().size(); i++) {
                    tm.setTranslation(math::Vec3f(t.x - getPoints()[i]->x, t.y - getPoints()[i]->y, 0));
                    *getPoints()[i] = tm.transform(*getPoints()[i]);
                }
                shadowOrigin += math::Vec3f(t.x, t.y, 0);
            }
            int Wall::getEntityId () {
                return getChildren()[0]->getId();
            }

            void Wall::setWallType (int type) {
                this->type = type;
            }
            int Wall::getWallType () {
                return type;
            }
            bool Wall::operator== (Entity &other) {
                if (other.getType() != "E_WALL")
                    return false;
                Wall &wall = static_cast<Wall&> (other);
                return type == wall.type && shadowType == wall.shadowType;
            }
            void Wall::setShadowCenter (math::Vec3f shadCenter) {

                this->shadowCenter =  shadCenter;
            }
            math::Vec3f Wall::getShadowCenter() {
                return shadowCenter;
            }
            void Wall::setShadowScale(math::Vec3f shadowScale) {
                this->shadowScale = shadowScale;
            }
            math::Vec3f Wall::getShadowScale() {
                return shadowScale;
            }
            void Wall::setShadowRotation(float angle, math::Vec3f axis) {
                this->shadowRotationAngle = shadowRotationAngle;
                this->shadowRotationAxis = shadowRotationAxis;
            }
            math::Vec3f Wall::getShadowRotationAxis() {
                return shadowRotationAxis;
            }
            float Wall::getShadowRotationAngle() {
                return shadowRotationAngle;
            }
            void Wall::setShadowOrigin(math::Vec3f origin) {
                shadowOrigin = origin;
            }
            math::Vec3f Wall::getShadowOrigin() {
                return shadowOrigin;
            }
            bool Wall::selectable () const {
                return true;
            }
            bool Wall::isLight() const {
                return false;
            }
            bool Wall::isShadow() const {
                return false;
            }
            Wall::~Wall () {


            }
        }
    }
}



