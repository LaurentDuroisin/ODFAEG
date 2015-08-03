#include "../../../include/odfaeg/Graphics/anim.h"
namespace odfaeg {
    namespace graphic {
        using namespace sf;
        using namespace std;
        Anim::Anim() : AnimatedEntity (math::Vec3f(0, 0, 0), math::Vec3f(0, 0, 0), math::Vec3f(0, 0, 0), "E_ANIMATION", nullptr) {
            currentFrameIndex = 0;
            running = false;
            currentFrameChanged = false;
            currentFrame = nullptr;
            previousFrame = nullptr;
            nextFrame = nullptr;
            interpLevels = 1;
            interpPerc = 0;
        }
        Anim::Anim (float fr, math::Vec3f position, math::Vec3f size, Entity *parent) : AnimatedEntity (position, size, size * 0.5f, "E_ANIMATION", parent) {
            this->fr = fr;
            currentFrameIndex = 0;
            running = false;
            currentFrameChanged = false;
            currentFrame = nullptr;
            previousFrame = nullptr;
            nextFrame = nullptr;
            interpLevels = 1;
            interpPerc = 0;
        }
        bool Anim::isCurrentFrameChanged() {
            return currentFrameChanged;
        }
        void Anim::setCurrentFrameChanged(bool b) {
            currentFrameChanged = b;
        }
        void Anim::addFrame (Entity *entity) {
            if (getChildren().size() == 0)
                currentFrame = entity;
            else if (getChildren().size() == 1)
                nextFrame = entity;
            entity->setParent(this);
            addChild(entity);
        }
        void Anim::removeFrame (Entity *entity) {
            removeChild(entity);
        }
        int Anim::getCurrentFrameIndex () {
            return currentFrameIndex;
        }
        void Anim::setCurrentFrame (int index) {
            if (getChildren().size() >= 2) {
                previousFrame = getChildren()[(index -1 < 0) ? 0 : index - 1];
                currentFrame = getChildren()[index];
                nextFrame = getChildren()[(index + 1 >= getChildren().size()) ? 0 : index + 1];
                currentFrameIndex = index;
                currentFrameChanged = true;
            }
        }
        Entity* Anim::getPreviousFrame() {
            return previousFrame;
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
        void Anim::computeNextFrame () {
            vas.clear();
            onFrameChanged();
        }
        void Anim::onFrameChanged() {
            if (getChildren().size() != 0) {
                for (unsigned int i = 0; i < getChildren().size(); i++) {
                    getChildren()[i]->onFrameChanged();
                }
            }
            if (getChildren().size() >= 2) {
                if (previousFrame == nullptr) {
                    previousFrame = getChildren()[(currentFrameIndex - 1 < 0) ? 0 : getChildren().size() - 1];
                }
                interpPerc++;
                if (interpPerc >= interpLevels) {
                    previousFrame = getChildren()[currentFrameIndex];
                    currentFrameChanged = true;
                    currentFrameIndex++;
                    if (currentFrameIndex >= getChildren().size()) {
                        currentFrameIndex = 0;
                        if (!loop) {
                            running = false;
                        }
                    }
                    currentFrame = getChildren()[currentFrameIndex];
                    nextFrame = getChildren()[(currentFrameIndex + 1 >= getChildren().size()) ? 0 : currentFrameIndex+1];
                    interpPerc = 0;
                }
                interpolate(currentFrame, nextFrame);
            }
        }
        void Anim::interpolate(Entity* currentFrame, Entity* nextFrame) {
            if (currentFrame->getChildren().size() == nextFrame->getChildren().size()) {
                if (currentFrame->getChildren().size() != 0 && nextFrame->getChildren().size() != 0) {
                    for (unsigned int i = 0; i < currentFrame->getChildren().size(); i++) {
                         interpolate(currentFrame->getChildren()[i], nextFrame->getChildren()[i]);
                    }
                }
                if (currentFrame->getFaces().size() == nextFrame->getFaces().size()) {
                    for (unsigned int i = 0; i < currentFrame->getFaces().size(); i++) {
                        VertexArray cva = currentFrame->getFaces()[i]->getVertexArray();
                        const VertexArray& nva = nextFrame->getFaces()[i]->getVertexArray();
                        if (cva.getVertexCount() == nva.getVertexCount()) {
                            for (unsigned int j = 0; j < cva.getVertexCount(); j++) {

                                cva[j].position.x = cva[j].position.x + (nva[j].position.x - cva[j].position.x) * (interpPerc / interpLevels);
                                cva[j].position.y = cva[j].position.y + (nva[j].position.y - cva[j].position.y) * (interpPerc / interpLevels);
                                cva[j].position.z = cva[j].position.z + (nva[j].position.z - cva[j].position.z) * (interpPerc / interpLevels);

                            }
                        }
                        if (interpLevels > 1)
                            cva.updateVBOBuffer();
                        vas.push_back(cva);
                    }
                }
            }
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
        Entity* Anim::getCurrentFrame () const {
              return currentFrame;
        }

        void Anim::onDraw(RenderTarget &target, RenderStates states) {
            for (unsigned int i = 0; i < vas.size(); i++)
                target.draw(vas[i], states);
        }
        Anim::~Anim () {


        }
    }
}

