#ifndef ODFAEG_LIGHT_COMPONENT
#define ODFAEG_LIGHT_COMPONENT
#include "component.h"
namespace odfaeg {
    namespace graphic {
        class LightComponent : public Component {
            public :
            LightComponent(RenderWindow& window, math::Vec3f position, math::Vec3f size, math::Vec3f origin, unsigned int priority = 0, LightComponent* parent = nullptr) :
                Component (window, position, size, origin, priority), parent(parent) {

            }
            void setParent(LightComponent* parent) {
                this->parent = parent;
            }
            LightComponent* getParent() {
                return parent;
            }
            void recomputeSize() {
                float sx, sy, npx, npy, nsx, nsy;
                sx = getSize().x;
                sy = getSize().y;
                if (parent != nullptr) {
                    npx = parent->getPosition().x + parent->getSize().x * getRelPosition().x;
                    npy = parent->getPosition().y + parent->getSize().y * getRelPosition().y;
                    nsx = parent->getSize().x * getRelSize().x;
                    nsy = parent->getSize().y * getRelSize().y;
                } else {
                    npx = getWindow().getSize().x * getRelPosition().x;
                    npy = getWindow().getSize().y * getRelPosition().y;
                    nsx = getWindow().getSize().x * getRelSize().x;
                    nsy = getWindow().getSize().y * getRelSize().y;
                }
                setScale(math::Vec3f(nsx / sx, nsy / sy, 1.f));
                setPosition(math::Vec3f(npx, npy, getPosition().z));
            }
            void draw(RenderTarget& target, RenderStates states) {
                states.transform = getTransform();
                onDraw(target, states);
                for (unsigned int i = 0; i < children.size(); i++) {
                    children[i]->draw(target, states);
                }
            }
            virtual void onDraw(RenderTarget &target, RenderStates states) {}
            virtual void addChild(LightComponent* child) {
                std::unique_ptr<LightComponent> ptr;
                ptr.reset(child);
                if (ptr->isAutoResized())
                    ptr->recomputeSize();
                children.push_back(std::move(ptr));
            }
            std::vector<LightComponent*> getChildren() {
                std::vector<LightComponent*> chlds;
                for (unsigned int i = 0; i < children.size(); i++) {
                    chlds.push_back(children[i].get());
                }
                return chlds;
            }
            void onUpdate(RenderWindow* rw, sf::Event& event) {
                for (unsigned int i = 0; i < children.size(); i++) {
                    children[i]->onUpdate(rw, event);
                }
            }
            virtual void pushEvent(sf::Event event, RenderWindow &rw) {
                if (event.type == sf::Event::Resized && &getWindow() == &rw && isAutoResized()) {
                    recomputeSize();
                    getListener().pushEvent(event);
                    for (unsigned int i = 0; i < children.size(); i++) {
                        children[i]->pushEvent(event, rw);
                        if (children[i]->isAutoResized()) {
                            children[i]->recomputeSize();
                        }
                    }
                }
                onEventPushed(event, rw);
            }
            void processEvents() {
                getListener().processEvents();
                for (unsigned int i = 0; i < children.size(); i++) {
                    children[i]->getListener().processEvents();
                }
            }
            virtual void removeAll() {
                children.clear();
            }
            virtual void onEventPushed(sf::Event event, RenderWindow& window) {
            }
            virtual void checkSubWindowEvents() = 0;
            virtual ~LightComponent() {}
            private :
            LightComponent* parent;
            std::vector<std::unique_ptr<LightComponent>> children;
        };
    }
}
#endif // ODFAEG_LIGHT_COMPONENT
