#include "../../../include/odfaeg/Graphics/anim.h"
namespace odfaeg {
    namespace graphic {
        using namespace sf;
        using namespace std;
        Anim::Anim() : AnimatedEntity (math::Vec3f(0, 0, 0), math::Vec3f(0, 0, 0), math::Vec3f(0, 0, 0), "E_ANIMATION", nullptr) {

        }
        Anim::Anim (float fr, math::Vec3f position, math::Vec3f size, Entity *parent) : AnimatedEntity (position, size, size * 0.5f, "E_ANIMATION", parent) {
            this->fr = fr;
            currentTile = 0;
            running = false;
            currentEntityChanged = false;
            currentEntity = nullptr;
            previousCurrentEntity = nullptr;
        }
        bool Anim::isCurrentEntityChanged() {
            return currentEntityChanged;
        }
        void Anim::setCurrentEntityChanged(bool b) {
            currentEntityChanged = b;
        }
        void Anim::addEntity (Entity *entity) {
            if (getChildren().size() == 0)
                currentEntity = entity;
            entity->setParent(this);
            addChild(entity);
        }
        /*void Anim::removeEntity (Entity *entity) {
            removeChild(entity);
        }*/
        int Anim::getCurrentTileIndex () {
            return currentTile;
        }
        void Anim::setCurrentTile (int index) {
            previousCurrentEntity = getChildren()[currentTile];
            currentTile = index;
            currentEntity = getChildren()[index];
            currentEntityChanged = true;
        }
        Entity* Anim::getPreviousCurrentEntity() {
            return previousCurrentEntity;
        }
        void Anim::setFrameRate (float fr) {
            this->fr = fr;
        }
        bool Anim::isRunning () {
            return running;
        }
        void Anim::play (bool loop) {
            if (getChildren().size() > 1 && !running) {
                running = true;
                this->loop = loop;
            }
        }
        void Anim::stop (){
            if (running) {
                running = false;
            }
        }
        void Anim::nextImage () {
            previousCurrentEntity = getChildren()[currentTile];
            currentEntityChanged = true;
            currentTile++;
            if (currentTile >= getChildren().size()) {
                currentTile = 0;
                if (!loop) {
                    running = false;
                }
            }
            currentEntity = getChildren()[currentTile];
        }
        Time Anim::getElapsedTime () {
            return clock.getElapsedTime();
        }
        void Anim::resetClock () {
            clock.restart();
        }
        bool Anim::operator== (Entity &other) {

            if (other.getType() != "E_ANIMATION")
                return false;
            Anim &a = static_cast<Anim&> (other);
            if (getPosition().x != a.getPosition().x || getPosition().y != a.getPosition().y
                || getSize().x != a.getSize().x || getSize().y != a.getSize().y)
                return false;
            for (unsigned int i = 0; i < getChildren().size(); i++) {
                if (!(*getChildren()[i] == *a.getChildren()[i]))
                    return false;
            }
            return true;
        }

        float Anim::getFrameRate () {
            return fr;
        }
        Entity* Anim::getCurrentEntity () const {
              return currentEntity;
        }

        void Anim::onDraw(RenderTarget &target, RenderStates states) {
            target.draw(*getChildren()[currentTile], states);
        }

        Anim::~Anim () {


        }
    }
}

