#include "../../../include/odfaeg/Graphics/renderComponentManager.h"
#include "../../../include/odfaeg/Graphics/GUI/menu.hpp"
#include "../../../include/odfaeg/Core/command.h"
namespace odfaeg {
    namespace graphic {
        RenderComponentManager::RenderComponentManager(RenderWindow& window) {
            windows.push_back(&window);
        }
        void RenderComponentManager::addWindow(RenderWindow& window) {
            windows.push_back(&window);
        }
        void RenderComponentManager::addComponent(Component* component) {
            std::cout<<"add component layer : "<<component->getPriority()<<std::endl;
            components.insert(std::make_pair(component->getPriority(), component));
        }
        bool RenderComponentManager::removeComponent(unsigned int layer) {
            std::multimap<int, Component*, std::less<int>>::iterator it;
            for (it = components.begin(); it != components.end();) {
                if (it->second->getId() == layer) {
                    it = components.erase(it);
                    return true;
                } else {
                    it++;
                }
            }
            return false;
        }
        RenderWindow& RenderComponentManager::getWindow() {
            return *windows[0];
        }
        unsigned int RenderComponentManager::getNbComponents() {
            return components.size();
        }

        void RenderComponentManager::drawRenderComponents() {
            std::multimap<int, Component*, std::less<int>>::iterator it;
            for (it = components.begin(); it != components.end(); it++) {
                if (dynamic_cast<HeavyComponent*>(it->second) != nullptr && it->second->isVisible()) {
                    for (unsigned int i = 0; i < windows.size(); i++) {
                        if (windows[i] == &it->second->getWindow()) {
                            windows[i]->clearDepth();
                            it->second->getWindow().draw(*it->second);
                        }
                    }
                }
            }
        }
        void RenderComponentManager::drawGuiComponents() {
            std::multimap<int, Component*, std::less<int>>::iterator it;
            for (it = components.begin(); it != components.end(); it++) {
                if (dynamic_cast<LightComponent*>(it->second) != nullptr && it->second->isVisible()) {
                    for (unsigned int i = 0; i < windows.size(); i++) {
                        if (windows[i] == &it->second->getWindow()) {
                            windows[i]->clearDepth();
                            View view = it->second->getWindow().getView();
                            View defaultView = it->second->getWindow().getDefaultView();
                            defaultView.setCenter(math::Vec3f(it->second->getWindow().getSize().x * 0.5f, it->second->getWindow().getSize().y * 0.5f, 0));
                            it->second->getWindow().setView(defaultView);
                            it->second->getWindow().draw(*it->second);
                            it->second->getWindow().setView(view);
                        }
                    }
                }
            }
        }
        HeavyComponent* RenderComponentManager::getRenderComponent(unsigned int layer) {
            std::multimap<int, Component*, std::less<int>>::iterator it;
            unsigned int i = 0;
            for (it = components.begin(); it != components.end(); it++) {
               if (i == layer && dynamic_cast<HeavyComponent*>(it->second) != nullptr) {
                   return dynamic_cast<HeavyComponent*>(it->second);
               }
               i++;
            }
            return nullptr;
        }
        LightComponent* RenderComponentManager::getGuiComponent(unsigned int layer) {
            std::multimap<int, Component*, std::less<int>>::iterator it;
            unsigned int i = 0;
            for (it = components.begin(); it != components.end(); it++) {
               if (i == layer && dynamic_cast<LightComponent*>(it->second) != nullptr) {
                   return dynamic_cast<LightComponent*>(it->second);
               }
               i++;
            }
            return nullptr;
        }
        bool RenderComponentManager::isComponentAdded(unsigned int layer) {
           std::multimap<int, Component*, std::less<int>>::iterator it;
           for (it = components.begin(); it != components.end(); it++) {
               if (it->second->getId() == layer) {
                   return true;
               }
           }
           return false;
        }
        Component* RenderComponentManager::getComponent(unsigned int layer) {
           std::multimap<int, Component*, std::less<int>>::iterator it;
           unsigned int i = 0;
           for (it = components.begin(); it != components.end(); it++) {
               if (i == layer) {
                    return it->second;
               }
               i++;
           }
        }
        void RenderComponentManager::clearComponents() {
            std::multimap<int, Component*, std::less<int>>::iterator it;
            for (it = components.begin(); it != components.end(); it++) {
                if (it->second->isVisible()) {
                    it->second->clear();
                }
            }
        }
        void RenderComponentManager::updateComponents() {
           std::multimap<int, Component*, std::less<int>>::iterator it;
           for (it = components.begin(); it != components.end(); it++) {
               if (it->second->isEventContextActivated()) {
                   it->second->processEvents();
                   it->second->recomputeSize();
               }
           }
           core::Command::clearEventsStack();
        }
        RenderComponentManager::~RenderComponentManager() {
           std::multimap<int, Component*, std::less<int>>::iterator it;
           for (it = components.begin(); it != components.end(); it++) {
                delete it->second;
           }
        }
    }
}
