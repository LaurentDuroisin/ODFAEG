#ifndef ODFAEG_LIGHT_COMPONENT
#define ODFAEG_LIGHT_COMPONENT
#include "component.h"
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include "glCheck.h"
namespace odfaeg {
    namespace graphic {
        class LightComponent : public Component {
            public :
            LightComponent(RenderWindow& window, math::Vec3f position, math::Vec3f size, math::Vec3f origin, unsigned int priority = 0, LightComponent* parent = nullptr) :
                Component (window, position, size, origin, priority), parent(parent) {
            }
            void setName(std::string name) {
                if (name == "PINVENTORY")
                    Transformable::setName(name);
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
                    unsigned int sx, sy, npx, npy, nsx, nsy, psx, psy, ppx, ppy;
                    /*sx = getSize().x;
                    sy = getSize().y;*/
                    if (parent != nullptr) {
                        npx = parent->getPosition().x + parent->getSize().x * getRelPosition().x;
                        npy = parent->getPosition().y + parent->getSize().y * getRelPosition().y;
                        nsx = parent->getSize().x * getRelSize().x;
                        nsy = parent->getSize().y * getRelSize().y;
                        psx = parent->getSize().x;
                        psy = parent->getSize().y;
                        ppx = parent->getPosition().x;
                        ppy = parent->getPosition().y;
                    } else {
                        npx = getWindow().getSize().x * getRelPosition().x;
                        npy = getWindow().getSize().y * getRelPosition().y;
                        nsx = getWindow().getSize().x * getRelSize().x;
                        nsy = getWindow().getSize().y * getRelSize().y;
                        psx = getWindow().getSize().x;
                        psy = getWindow().getSize().y;
                        ppx = getWindow().getPosition().x;
                        ppy = getWindow().getPosition().y;
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
                //states.transform = getTransform();
                getWindow().setActive(true);
                if (getWindow().getName() == "WAPPLICATIONNEW") {
                    GLboolean* params =  new GLboolean[1];
                    glGetBooleanv(GL_SCISSOR_TEST, params);
                    if (params[0] == GL_TRUE)
                       std::cout<<getWindow().getName()<<" true"<<std::endl;
                    else
                       std::cout<<getWindow().getName()<<" false"<<std::endl;
                }
                onDraw(target, states);
                std::multimap<int, LightComponent*, std::greater<int>> sortedChildren;
                for (unsigned int i = 0; i < children.size(); i++) {
                    sortedChildren.insert(std::make_pair(children[i]->getPriority(), children[i].get()));
                }
                std::multimap<int, LightComponent*, std::greater<int>>::iterator it;
                for (it = sortedChildren.begin(); it != sortedChildren.end(); it++) {
                    if (it->second->isVisible()
                        && it->second->getPosition().x + it->second->getSize().x >= getPosition().x
                        && it->second->getPosition().y + it->second->getSize().y >= getPosition().y
                        && it->second->getPosition().x <= getPosition().x + getSize().x
                        && it->second->getPosition().y <= getPosition().y + getSize().y) {
                        /*if(name == "PFILES" && it == sortedChildren.begin()) {
                            std::cout<<"child pos : "<<it->second->getPosition()<<" child size : "<<it->second->getSize()<<std::endl;
                        }*/
                        it->second->draw(target, states);
                    }
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
            virtual void onUpdate(RenderWindow* rw, window::IEvent& event) {
                for (unsigned int i = 0; i < children.size(); i++) {
                    children[i]->onUpdate(rw, event);
                }
            }
            virtual void pushEvent(window::IEvent event, RenderWindow &rw) {
                getListener().pushEvent(event);
                onEventPushed(event, rw);
                for (unsigned int i = 0; i < children.size(); i++) {
                    children[i]->pushEvent(event, rw);
                }
            }
            virtual void processEvents() {
                if (isEventContextActivated()) {
                    getListener().processEvents();
                    for (unsigned int i = 0; i < children.size(); i++) {
                        children[i]->processEvents();
                    }
                }
            }
            virtual void removeAll() {
                children.clear();
            }
            virtual void onEventPushed(window::IEvent event, RenderWindow& window) {
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
