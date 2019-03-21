#include "../../../include/odfaeg/Graphics/zSortingRenderComponent.hpp"
namespace odfaeg {
    namespace graphic {
        ZSortingRenderComponent::ZSortingRenderComponent (RenderWindow& window, int layer, std::string expression, EntityManager& scene) :
            HeavyComponent(window, math::Vec3f(window.getView().getPosition().x, window.getView().getPosition().y, layer),
                          math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0),
                          math::Vec3f(window.getView().getSize().x + window.getView().getSize().x * 0.5f, window.getView().getPosition().y + window.getView().getSize().y * 0.5f, layer)),
            view(window.getView()),
            expression(expression),
            scene(scene) {
            update = false;
            core::FastDelegate<bool> signal (&ZSortingRenderComponent::needToUpdate, this);
            core::FastDelegate<void> slot (&ZSortingRenderComponent::drawNextFrame, this);
            core::Command cmd(signal, slot);
            getListener().connect("UPDATE", cmd);
        }
        void ZSortingRenderComponent::updateSceneVertices() {
            scene.getSceneVertices().updateVBOBuffer();
        }
        void ZSortingRenderComponent::loadShaders() {
            std::string vertexShader =
            "#version 130\n"
            "uniform mat4 transforms["+core::conversionUIntString(scene.getTransforms().size())+"];"
            "out float instanceId;"
            "void main () {"
            "   gl_Position = gl_ModelViewProjectionMatrix * transforms[int(gl_Normal.x)] * gl_Vertex;"
            "   gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;"
            "   gl_FrontColor = gl_Color;"
            "   instanceId = gl_Normal.x;"
            "}";
            std::string fragmentShader =
            "#version 130\n"
            "uniform float haveTexture;"
            "uniform sampler2D texture;"
            "in float instanceId;"
            "void main() {"
                "vec4 colors[2];"
                "colors[0] = gl_Color;"
                "colors[1] = gl_Color * texture2D(texture, gl_TexCoord[0].xy);"
                "bool b = (haveTexture > 0.9);"
                "gl_FragColor = colors[int(b)];"
            "}";
            if (!instancedRenderingShader.loadFromMemory(vertexShader, Shader::Vertex)) {
                throw core::Erreur(50, "Failed to load instanced rendering shader", 0);
            }
            //instancedRenderingShader.setParameter("texture", Shader::CurrentTexture);
            instancedRenderingShader.setParameter("transforms", scene.getTransforms());
        }
        void ZSortingRenderComponent::updateTransformMatrices () {
            std::cout<<"update transform matrices"<<std::endl;
            instancedRenderingShader.setParameter("transforms", scene.getTransforms());
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
                         VertexArray& va = vEntities[i]->getFace(j)->getVertexArray();
                         /*for (unsigned int n = 0; n < va.getVertexCount(); n++) {
                            std::cout<<"index : "<<*va.m_indexes[n]<<std::endl;
                         }*/
                         batcher.addFace( vEntities[i]->getFace(j), scene.getSceneVertices().m_indexes.size());
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
                    //states.shader = &instancedRenderingShader;
                    /*for (unsigned int j = 0; j < m_instances[i].getAllIndexes().size(); j++)
                        std::cout<<"index : "<< m_instances[i].getAllIndexes()[j];*/
                    /*if (m_instances[i].getMaterial().getTexture() == nullptr) {
                        instancedRenderingShader.setParameter("haveTexture", 0);
                    } else {
                        instancedRenderingShader.setParameter("haveTexture", 1);
                    }*/
                    scene.getSceneVertices().setPrimitiveType(m_instances[i].getPrimitiveType());
                    if (update) {
                        scene.getSceneVertices().resetIndexes(m_instances[i].getAllIndexes());
                       // update = false;
                    }
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
