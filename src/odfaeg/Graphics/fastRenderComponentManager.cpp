#include "../../../include/odfaeg/Graphics/fastRenderComponentManager.h"
namespace odfaeg {
    namespace graphic {
        FastRenderComponentManager::FastRenderComponentManager(RenderWindow& window) : window (window) {

        }
        void FastRenderComponentManager::addComponent(Component* component) {
             components.push_back(component);
        }
        bool FastRenderComponentManager::removeComponent(unsigned int layer) {
            std::vector<Component*>::iterator it;
            for (it = components.begin(); it != components.end();) {
                if ((*it)->getId() == layer) {
                    it = components.erase(it);
                    return true;
                } else {
                    it++;
                }
            }
            return false;
        }
        RenderWindow& FastRenderComponentManager::getWindow() {
            return window;
        }
        unsigned int FastRenderComponentManager::getNbComponents() {
            return components.size();
        }

        void FastRenderComponentManager::drawRenderComponents() {
            for (unsigned int i = 0; i < components.size(); i++) {
                if (getRenderComponent(i) != nullptr && getRenderComponent(i)->isVisible())
                    window.draw(*components[i]);
            }
        }
        void FastRenderComponentManager::drawGuiComponents() {
            View view = window.getView();
            View defaultView = window.getDefaultView();
            defaultView.setCenter(math::Vec3f(window.getSize().x * 0.5f, window.getSize().y * 0.5f, 0));
            window.setView(defaultView);
            for (unsigned int i = 0; i < components.size(); i++) {
                if (getRenderComponent(i) == nullptr) {
                    static_cast<LightComponent*>(components[i])->checkSubWindowEvents();
                    if (getGuiComponent(i)->isVisible())
                        window.draw(*components[i]);
                }
            }
            window.setView(view);
        }
        LightComponent* FastRenderComponentManager::getGuiComponent(unsigned int layer) {
            return dynamic_cast<LightComponent*>(components[layer]);
        }
        bool FastRenderComponentManager::isComponentCreated(unsigned int layer) {
           if (layer >= components.size())
               return false;
           return true;
        }
        HeavyComponent* FastRenderComponentManager::getRenderComponent(unsigned int layer) {
            return dynamic_cast<HeavyComponent*>(components[layer]);
        }
        Component* FastRenderComponentManager::getComponent(unsigned int layer) {
            return components[layer];
        }
        void FastRenderComponentManager::clearComponents() {
            for (unsigned int i = 0; i < components.size(); i++) {
                if(components[i]->isVisible())
                    components[i]->clear();
            }
        }
        void FastRenderComponentManager::updateComponents() {
            for (unsigned int i = 0; i < components.size(); i++) {
                if (components[i]->isEventContextActivated() && !components[i]->getListener().isUsingThread()) {
                    components[i]->getListener().processEvents();
                }
            }
        }
        FastRenderComponentManager::~FastRenderComponentManager() {
            for (unsigned int i = 0; i < components.size(); i++) {
                delete components[i];
            }
        }
    }
}
