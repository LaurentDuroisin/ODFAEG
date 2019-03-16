#include "../../../include/odfaeg/Graphics/zSortingRenderComponent.hpp"
namespace odfaeg {
    namespace graphic {
        ZSortingRenderComponent::ZSortingRenderComponent (RenderWindow& window, int layer, std::string expression) :
            HeavyComponent(window, math::Vec3f(window.getView().getPosition().x, window.getView().getPosition().y, layer),
                          math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0),
                          math::Vec3f(window.getView().getSize().x + window.getView().getSize().x * 0.5f, window.getView().getPosition().y + window.getView().getSize().y * 0.5f, layer)),
            view(window.getView()),
            expression(expression) {
            update = false;
            core::FastDelegate<bool> signal (&ZSortingRenderComponent::needToUpdate, this);
            core::FastDelegate<void> slot (&ZSortingRenderComponent::drawNextFrame, this);
            core::Command cmd(signal, slot);
            getListener().connect("UPDATE", cmd);
        }
        void ZSortingRenderComponent::pushEvent(window::IEvent event, RenderWindow& rw) {
            if (event.type == window::IEvent::WINDOW_EVENT && event.window.type == window::IEvent::WINDOW_EVENT_RESIZED && &getWindow() == &rw && isAutoResized()) {
                std::cout<<"recompute size"<<std::endl;
                recomputeSize();
                getListener().pushEvent(event);
                getView().reset(physic::BoundingBox(getView().getViewport().getPosition().x, getView().getViewport().getPosition().y, getView().getViewport().getPosition().z, event.window.data1, event.window.data2, getView().getViewport().getDepth()));
            }
        }
        bool ZSortingRenderComponent::needToUpdate() {
            return update;
        }
        void ZSortingRenderComponent::changeVisibleEntities(Entity* toRemove, Entity* toAdd, EntityManager* em) {
            bool removed;
            em->removeAnimatedVisibleEntity(toRemove, visibleEntities, view, removed);
            if (removed) {
                em->insertAnimatedVisibleEntity(toAdd, visibleEntities, view);
                loadEntitiesOnComponent(visibleEntities);
                update = true;
            }
        }
        std::string ZSortingRenderComponent::getExpression() {
            return expression;
        }
        bool ZSortingRenderComponent::loadEntitiesOnComponent(std::vector<Entity*> vEntities)
        {

            batcher.clear();
            for (unsigned int i = 0; i < vEntities.size(); i++) {
                if ( vEntities[i]->isLeaf()) {
                    for (unsigned int j = 0; j <  vEntities[i]->getNbFaces(); j++) {
                         batcher.addFace( vEntities[i]->getFace(j));
                    }
                }
            }
            m_instances = batcher.getInstances();
            visibleEntities = vEntities;
            update = true;
            return true;
        }
        void ZSortingRenderComponent::cutIntersectingVA() {
            /*for (unsigned int i = 0; i < m_instances.size(); i++) {
                for (unsigned int j = 0; j < m_instances[i]->getVertexArrays().size(); j++) {
                    VertexArray va1 = *m_instances[i]->getVertexArrays()[j];
                    for (unsigned int k = 0; k < m_instances.size(); k++) {
                        for (unsigned int l = 0; l < m_instances[k]->getVertexArrays().size(); l++) {
                            VertexArray va2 = *m_instances[k]->getVertexArrays()[j];
                            if (va1 != va2) {
                                for (unsigned int m = 0; m < va1.getVertexCount(); m++) {
                                    Vertex v1 = va1[m];
                                    Vertex v2 = va1[(m < va1.getVertexCount().size() - 1 ) ? m + 1 : 0];
                                    odfaeg::physic::Ray r1 = odfaeg::physic::Ray (Vec3f(v1.position.x, v1.position.y,v1.position.z), Vec3f(v2.position.x, v2.position.y, v2.position.z));
                                }
                            }
                        }
                    }
                }
            }*/
        }
        void ZSortingRenderComponent::setView(View view) {
            this->view = view;
        }
        void ZSortingRenderComponent::setExpression(std::string expression) {
            update = true;
            this->expression = expression;
        }
        std::vector<Entity*> ZSortingRenderComponent::getEntities() {
            return visibleEntities;
        }
        void ZSortingRenderComponent::draw(RenderTarget& target, RenderStates states) {
            states.blendMode = sf::BlendAlpha;
            for (unsigned int i = 0; i < m_instances.size(); i++) {
                if (m_instances[i].getAllVertices().getVertexCount() > 0) {
                    m_instances[i].sortVertexArrays(view);
                    states.texture = m_instances[i].getMaterial().getTexture();
                    target.draw(m_instances[i].getAllVertices(), states);
                }
            }
        }
        View& ZSortingRenderComponent::getView() {
            return view;
        }
        int ZSortingRenderComponent::getLayer() {
            return getPosition().z;
        }
        void ZSortingRenderComponent::updateParticleSystems() {
            /*for (unsigned int i = 0; i < visibleEntities.size(); i++) {
                if (dynamic_cast<physic::ParticleSystem*>(visibleEntities[i]) != nullptr) {
                    static_cast<physic::ParticleSystem*>(visibleEntities[i])->update();
                }
            }
            loadEntitiesOnComponent(visibleEntities);
            update = true;*/
        }
    }
}
