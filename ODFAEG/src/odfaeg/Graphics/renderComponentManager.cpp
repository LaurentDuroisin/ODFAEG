#include "../../../include/odfaeg/Graphics/renderComponentManager.h"
#include "../../../include/odfaeg/Core/command.h"
namespace odfaeg {
    namespace graphic {
        RenderComponentManager::RenderComponentManager(RenderWindow& window) : window (window) {

        }
        void RenderComponentManager::addComponent(Component* component) {
            components.insert(std::make_pair(component->getPriority(), component));
        }
        bool RenderComponentManager::removeComponent(unsigned int layer) {
            std::multimap<unsigned int, Component*, std::greater<unsigned int>>::iterator it;
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
            return window;
        }
        unsigned int RenderComponentManager::getNbComponents() {
            return components.size();
        }

        void RenderComponentManager::drawRenderComponents() {
            std::multimap<unsigned int, Component*, std::greater<unsigned int>>::iterator it;
            for (it = components.begin(); it != components.end(); it++) {
                if (dynamic_cast<HeavyComponent*>(it->second) != nullptr && it->second->isVisible()) {
                    it->second->getWindow().draw(*it->second);
                }
            }
        }
        void RenderComponentManager::drawGuiComponents() {
            View view = window.getView();
            View defaultView = window.getDefaultView();
            defaultView.setCenter(math::Vec3f(window.getSize().x * 0.5f, window.getSize().y * 0.5f, 0));
            window.setView(defaultView);
            std::multimap<unsigned int, Component*, std::greater<unsigned int>>::iterator it;
            for (it = components.begin(); it != components.end(); it++) {
                if (dynamic_cast<LightComponent*>(it->second) != nullptr && it->second->isVisible()) {
                    static_cast<LightComponent*>(it->second)->checkSubWindowEvents();
                    it->second->getWindow().draw(*it->second);
                }
            }
            window.setView(view);
        }
        HeavyComponent* RenderComponentManager::getRenderComponent(unsigned int layer) {
            std::multimap<unsigned int, Component*, std::greater<unsigned int>>::iterator it;
            unsigned int i = 0;
            for (it = components.begin(); it != components.end(); it++) {
               if (i == layer) {
                   return dynamic_cast<HeavyComponent*>(it->second);
               }
               i++;
            }
            return nullptr;
        }
        LightComponent* RenderComponentManager::getGuiComponent(unsigned int layer) {
            std::multimap<unsigned int, Component*, std::greater<unsigned int>>::iterator it;
            unsigned int i = 0;
            for (it = components.begin(); it != components.end(); it++) {
               if (i == layer) {
                   return dynamic_cast<LightComponent*>(it->second);
               }
               i++;
            }
            return nullptr;
        }
        bool RenderComponentManager::isComponentAdded(unsigned int layer) {
           std::multimap<unsigned int, Component*, std::greater<unsigned int>>::iterator it;
           for (it = components.begin(); it != components.end(); it++) {
               if (it->second->getId() == layer) {
                   return true;
               }
           }
           return false;
        }
        Component* RenderComponentManager::getComponent(unsigned int layer) {
           std::multimap<unsigned int, Component*, std::greater<unsigned int>>::iterator it;
           unsigned int i = 0;
           for (it = components.begin(); it != components.end(); it++) {
               if (i == layer) {
                    return it->second;
               }
               i++;
           }
        }
        void RenderComponentManager::clearComponents() {
            std::multimap<unsigned int, Component*, std::greater<unsigned int>>::iterator it;
            for (it = components.begin(); it != components.end(); it++) {
                if (it->second->isVisible()) {
                    it->second->clear();
                }
            }
        }
        void RenderComponentManager::updateComponents() {
           std::multimap<unsigned int, Component*, std::greater<unsigned int>>::iterator it;
           for (it = components.begin(); it != components.end(); it++) {
               if (it->second->isEventContextActivated()) {
                   it->second->getListener().processEvents();
               }
           }
           core::Command::clearEventsStack();
        }
        RenderComponentManager::~RenderComponentManager() {
           std::multimap<unsigned int, Component*, std::greater<unsigned int>>::iterator it;
           for (it = components.begin(); it != components.end(); it++) {
                delete it->second;
           }
        }
    }
}
