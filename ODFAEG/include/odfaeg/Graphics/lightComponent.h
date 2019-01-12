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
            void setName(std::string name) {
                this->name = name;
            }
            std::string getName() {
                return name;
            }
            void setParent(LightComponent* parent) {
                this->parent = parent;
            }
            LightComponent* getParent() {
                return parent;
            }
            void recomputeSize() {
                if (isAutoResized()) {
                    float sx, sy, npx, npy, nsx, nsy, psx, psy;
                    /*sx = getSize().x;
                    sy = getSize().y;*/
                    if (parent != nullptr) {
                        npx = parent->getPosition().x + parent->getSize().x * getRelPosition().x;
                        npy = parent->getPosition().y + parent->getSize().y * getRelPosition().y;
                        nsx = parent->getSize().x * getRelSize().x;
                        nsy = parent->getSize().y * getRelSize().y;
                        psx = parent->getSize().x;
                        psy = parent->getSize().y;
                    } else {
                        npx = getWindow().getSize().x * getRelPosition().x;
                        npy = getWindow().getSize().y * getRelPosition().y;
                        nsx = getWindow().getSize().x * getRelSize().x;
                        nsy = getWindow().getSize().y * getRelSize().y;
                        psx = getWindow().getSize().x;
                        psy = getWindow().getSize().y;
                    }
                    setSize(math::Vec3f(nsx, nsy, 0.f));
                    setPosition(math::Vec3f(npx, npy, getPosition().z));
                    if (getSize().x > psx)
                        setSize(math::Vec3f(psx, getSize().y, 0));
                    if (getSize().y > psy)
                        setSize(math::Vec3f(getSize().x, psy, 0));
                    setAutoResized(false);
                }
                for (unsigned int i = 0; i < children.size(); i++) {
                    children[i]->recomputeSize();
                }
                onSizeRecomputed();
            }
            void draw(RenderTarget& target, RenderStates states) {
                states.transform = getTransform();
                onDraw(target, states);
                for (unsigned int i = 0; i < children.size(); i++) {
                    if (children[i]->isVisible() && children[i]->getPosition().x >= getPosition().x && children[i]->getPosition().y >= getPosition().y
                    && children[i]->getPosition().x + children[i]->getSize().x <= getPosition().x + getSize().x
                    && children[i]->getPosition().y + children[i]->getSize().y <= getPosition().y + getSize().y)
                        children[i]->draw(target, states);
                }
                drawOn(target, states);
            }
            virtual void onDraw(RenderTarget &target, RenderStates states) {}
            virtual void drawOn(RenderTarget &target, RenderStates states) {}
            virtual void onSizeRecomputed() {}
            virtual void addChild(LightComponent* child) {
                std::unique_ptr<LightComponent> ptr;
                ptr.reset(child);
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
                getListener().pushEvent(event);
                onEventPushed(event, rw);
                for (unsigned int i = 0; i < children.size(); i++) {
                    children[i]->pushEvent(event, rw);
                }
            }
            void processEvents() {
                getListener().processEvents();
                for (unsigned int i = 0; i < children.size(); i++) {
                    children[i]->processEvents();
                }
            }
            virtual void removeAll() {
                children.clear();
            }
            virtual void onEventPushed(sf::Event event, RenderWindow& window) {
            }
            bool removeChild(LightComponent& child)
            {
                const auto itChildToRemove =
                    std::find_if(children.begin(), children.end(),
                                 [&](auto& p) { return p.get() == &child; });

                const bool found = (itChildToRemove != children.end());
                if(found)
                    children.erase(itChildToRemove);
                return found;
            }
            virtual ~LightComponent() {}
            private :
            std::string name;
            LightComponent* parent;
            std::vector<std::unique_ptr<LightComponent>> children;
        };
    }
}
#endif // ODFAEG_LIGHT_COMPONENT
