#include "../../../include/odfaeg/Graphics/fastRenderComponentManager.h"
namespace odfaeg {
    namespace graphic {
        FastRenderComponentManager::FastRenderComponentManager(RenderWindow& window) : window (window) {

        }
        void FastRenderComponentManager::addRenderComponent(FastRenderComponent* component) {
            if (component->getPosition().z >= components.size())
                components.resize(component->getPosition().z+1);
            components[component->getPosition().z].push_back(component);
        }
        RenderWindow& FastRenderComponentManager::getWindow() {
            return window;
        }
        unsigned int FastRenderComponentManager::getNbComponents() {
            return components.size();
        }

        void FastRenderComponentManager::drawComponents() const {
            for (unsigned int l = 0; l < components.size(); l++) {
                for (unsigned int i = 0; i < components[l].size(); i++) {
                    window.draw(*components[l][i]);
                }
            }
        }
        bool FastRenderComponentManager::isComponentCreated(unsigned int layer) {
           if (layer >= components.size())
               return false;
           return true;
        }
        FastRenderComponent* FastRenderComponentManager::getRenderComponent(unsigned int layer) {
            return static_cast<FastRenderComponent*>(components[layer][0]);
        }
        void FastRenderComponentManager::clearComponents() {
            for (unsigned int l = 0; l < components.size(); l++) {
                for (unsigned int i = 0; i < components[l].size(); i++) {
                    components[l][i]->clear();
                }
            }
        }
        void FastRenderComponentManager::updateComponents() {
            for (unsigned int l = 0; l < components.size(); l++) {
                for (unsigned int i = 0; i < components[l].size(); i++) {
                    components[l][i]->getListener().processEvents();
                }
            }
        }
        FastRenderComponentManager::~FastRenderComponentManager() {
            for (unsigned int l = 0; l < components.size(); l++) {
                for (unsigned int i = 0; i < components[l].size(); i++) {
                    delete components[l][i];
                }
            }
        }
    }
}
