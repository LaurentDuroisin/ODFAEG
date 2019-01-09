#include "../../../include/odfaeg/Graphics/shadowRenderComponent.hpp"
#include "glCheck.h"
#include <memory.h>
using namespace sf;
using namespace std;
namespace odfaeg {
    namespace graphic {
        ShadowRenderComponent::ShadowRenderComponent (RenderWindow& window, int layer, std::string expression,sf::ContextSettings settings) :
            HeavyComponent(window, math::Vec3f(window.getView().getPosition().x, window.getView().getPosition().y, layer),
                          math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0),
                          math::Vec3f(window.getView().getSize().x + window.getView().getSize().x * 0.5f, window.getView().getPosition().y + window.getView().getSize().y * 0.5f, layer)),
            view(window.getView()),
            expression(expression) {
                update = false;
                sf::Vector3i resolution ((int) window.getSize().x, (int) window.getSize().y, window.getView().getSize().z);
                shadowMap = std::make_unique<RenderTexture>();
                stencilBuffer = std::make_unique<RenderTexture>();
                stencilBufferTile = std::make_unique<Tile>(&stencilBuffer->getTexture(), math::Vec3f(0, 0, 0), math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0), IntRect(0, 0, window.getView().getSize().x, window.getView().getSize().y));
                shadowTile = std::make_unique<Tile>(&shadowMap->getTexture(), math::Vec3f(0, 0, 0), math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0), IntRect(0, 0, window.getView().getSize().x, window.getView().getSize().y));
                core::FastDelegate<bool> signal (&ShadowRenderComponent::needToUpdate, this);
                core::FastDelegate<void> slot (&ShadowRenderComponent::drawNextFrame, this);
                core::Command cmd(signal, slot);
                getListener().connect("UPDATE", cmd);
                if (Shader::isAvailable()) {
                    shadowMap->create(resolution.x, resolution.y,settings);
                    settings.depthBits = 32;
                    stencilBuffer->create(resolution.x, resolution.y,settings);
                    buildShadowMapShader = std::make_unique<Shader>();
                    perPixShadowShader = std::make_unique<Shader>();
                    const std::string buildShadowMapVertexShader =
                        "#version 130 \n"
                        "out mat4 projMat;"
                        "void main () {"
                            "gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
                            "gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;"
                            "gl_FrontColor = gl_Color;"
                            "projMat = gl_ProjectionMatrix;"
                        "}";
                    const std::string buildShadowMapFragmentShader =
                        "#version 130 \n"
                        "uniform sampler2D texture;"
                        "uniform float haveTexture;"
                        "in mat4 projMat;"
                        "void main() {"
                        "   vec4 texel = texture2D(texture, gl_TexCoord[0].xy);"
                        "   vec4 colors[2];"
                        "   colors[1] = texel * gl_Color;"
                        "   colors[0] = gl_Color;"
                        "   bool b = (haveTexture == 1);"
                        "   float color = colors[int(b)].a;"
                        "   float z = (gl_FragCoord.w != 1.f) ? (inverse(projMat) * vec4(0, 0, 0, gl_FragCoord.w)).w : gl_FragCoord.z;"
                        "   gl_FragColor = vec4(0, 0, z, color);"
                        "}";
                    const std::string perPixShadowVertexShader =
                        "#version 130 \n"
                        "uniform mat4 depthBiasMatrix;"
                        "out vec4 shadowCoords;"
                        "out mat4 projMat;"
                        "void main () {"
                            "gl_Position = gl_ProjectionMatrix * gl_Vertex;"
                            "gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;"
                            "gl_FrontColor = gl_Color;"
                            "projMat = gl_ProjectionMatrix;"
                            "shadowCoords = depthBiasMatrix * vec4(gl_Position.xyz, 1);"
                        "}";
                    const std::string perPixShadowFragmentShader =
                        "#version 130 \n"
                        "uniform sampler2D texture;"
                        "uniform sampler2D stencilBuffer;"
                        "uniform float haveTexture;"
                        "in vec4 shadowCoords;"
                        "in mat4 projMat;"
                        "void main() {"
                        "   vec4 texel = texture2D(texture, gl_TexCoord[0].xy);"
                        "   vec4 colors[2];"
                        "   colors[1] = texel * gl_Color;"
                        "   colors[0] = gl_Color;"
                        "   bool b = (haveTexture == 1);"
                        "   float color = colors[int(b)].a;"
                        "   vec4 stencil = texture2D (stencilBuffer, shadowCoords.xy);"
                        "   float z = (gl_FragCoord.w != 1.f) ? (inverse(projMat) * vec4(0, 0, 0, gl_FragCoord.w)).w : gl_FragCoord.z;"
                        "   colors[1] = vec4 (0, 0, 0, color);"
                        "   colors[0] = vec4 (0.5, 0.5, 0.5, 0.5);"
                        "   b = (stencil.z < z);"
                        "   vec4 visibility = colors[int(b)];"
                        "   gl_FragColor = visibility;"
                        "}";
                    if (!buildShadowMapShader->loadFromMemory(buildShadowMapVertexShader, buildShadowMapFragmentShader)) {
                        throw core::Erreur(53, "Error, failed to load build shadow map shader", 3);
                    }
                    if (!perPixShadowShader->loadFromMemory(perPixShadowVertexShader, perPixShadowFragmentShader)) {
                        throw core::Erreur(54, "Error, failed to load per pix shadow map shader", 3);
                    }
                    buildShadowMapShader->setParameter("texture", Shader::CurrentTexture);
                    perPixShadowShader->setParameter("stencilBuffer", stencilBuffer->getTexture());
                    perPixShadowShader->setParameter("texture", Shader::CurrentTexture);
                } else {
                    throw core::Erreur(55, "Shader not supported!", 0);
                }
            }
            void ShadowRenderComponent::drawNextFrame() {
                math::Vec3f centerLight = g2d::AmbientLight::getAmbientLight().getLightCenter();
                View lightView = View(view.getSize().x, view.getSize().y, -g2d::AmbientLight::getAmbientLight().getHeight(), g2d::AmbientLight::getAmbientLight().getHeight());
                lightView.setCenter(centerLight);
                math::Vec3f forward = view.getPosition() - lightView.getPosition();
                math::Vec3f target = lightView.getPosition() + forward;
                lightView.lookAt(target.x, target.y, target.z);
                stencilBuffer->setView(lightView);
                math::Vec3f v = lightView.getPosition() - view.getPosition();
                RenderStates states;
                states.shader = buildShadowMapShader.get();
                physic::BoundingBox viewArea = view.getViewVolume();
                math::Vec3f position (viewArea.getPosition().x,viewArea.getPosition().y, view.getPosition().z);
                math::Vec3f size (viewArea.getWidth(), viewArea.getHeight(), 0);
                for (unsigned int i = 0; i < m_instances.size(); i++) {
                    states.texture = m_instances[i].getMaterial().getTexture();
                    if (m_instances[i].getMaterial().getTexture() != nullptr) {
                        buildShadowMapShader->setParameter("haveTexture", 1);
                    } else {
                        buildShadowMapShader->setParameter("haveTexture", 0);
                    }
                    stencilBuffer->draw(m_instances[i].getAllVertices(), states);
                }
                stencilBuffer->display();
                stencilBufferTile->setPosition(position);
                shadowMap->setView(view);
                math::Matrix4f biasMatrix(0.5f, 0.0f, 0.0f, 0.0f,
                                          0.0f, 0.5f, 0.0f, 0.0f,
                                          0.0f, 0.0f, 0.5f, 0.0f,
                                          0.5f, 0.5f, 0.5f, 1.f);
                math::Matrix4f depthBiasMatrix = biasMatrix * view.getViewMatrix().getMatrix() * view.getProjMatrix().getMatrix();
                perPixShadowShader->setParameter("depthBiasMatrix", depthBiasMatrix.transpose());
                states.shader = perPixShadowShader.get();
                for (unsigned int i = 0; i < m_shadow_instances.size(); i++) {
                    states.texture = m_shadow_instances[i].getMaterial().getTexture();
                    if (m_shadow_instances[i].getMaterial().getTexture() != nullptr) {
                        perPixShadowShader->setParameter("haveTexture", 1);
                    } else {
                        perPixShadowShader->setParameter("haveTexture", 0);
                    }
                    /*for (unsigned int j = 0; j < m_instances[i].getVertexArrays().size(); j++) {
                        states.transform = m_instances[i].getTransforms()[j];
                        math::Vec3f shadowOrigin, shadowCenter, shadowScale(1.f, 1.f, 1.f), shadowRotationAxis;
                        float shadowRotationAngle = 0;
                        Entity* entity = m_instances[i].getVertexArrays()[j].getEntity()->getParent();
                        if (entity->isModel()) {
                            shadowCenter = static_cast<Model*>(entity)->getShadowCenter();
                            shadowScale = static_cast<Model*>(entity)->getShadowScale();
                            shadowRotationAxis = static_cast<Model*>(entity)->getShadowRotationAxis();
                            shadowRotationAngle = static_cast<Model*>(entity)->getShadowRotationAngle();
                            shadowOrigin = static_cast<Model*>(entity)->getShadowOrigin();
                        }
                        TransformMatrix tm;
                        tm.setOrigin(entity->getPosition());
                        tm.setScale(shadowScale);
                        tm.setRotation(shadowRotationAxis, shadowRotationAngle);
                        tm.setTranslation(entity->getPosition() + shadowCenter);
                        tm.update();
                        perPixShadowShader->setParameter("shadowProjMat", tm.getMatrix().transpose());
                        shadowMap->draw(m_instances[i].getVertexArrays()[j], states);
                    }*/
                    shadowMap->draw(m_shadow_instances[i].getAllVertices(), states);
                }
                /*RectangleShape rect(size * 2.f);
                rect.setPosition(position - size * 0.5f);
                rect.setFillColor(sf::Color(100, 100, 100, 128));
                shadowMap->draw(rect);*/
                shadowMap->display();
            }
            std::vector<Entity*> ShadowRenderComponent::getEntities() {
                return visibleEntities;
            }
            void ShadowRenderComponent::draw(RenderTarget& target, RenderStates states) {
                shadowTile->setCenter(target.getView().getPosition());
                states.blendMode = sf::BlendMultiply;
                target.draw(*shadowTile, states);
            }
            void ShadowRenderComponent::pushEvent(sf::Event event, RenderWindow& rw) {
                if (event.type == sf::Event::Resized && &getWindow() == &rw && isAutoResized()) {
                    recomputeSize();
                    getListener().pushEvent(event);
                    getView().reset(physic::BoundingBox(getView().getViewport().getPosition().x, getView().getViewport().getPosition().y, getView().getViewport().getPosition().z, event.size.width, event.size.height, getView().getViewport().getDepth()));
                }
            }
            bool ShadowRenderComponent::needToUpdate() {
                return update;
            }
            View& ShadowRenderComponent::getView() {
                return view;
            }
            int ShadowRenderComponent::getLayer() {
                return getPosition().z;
            }
            const Texture& ShadowRenderComponent::getStencilBufferTexture() {
                return stencilBuffer->getTexture();
            }
            const Texture& ShadowRenderComponent::getShadowMapTexture() {
                return shadowMap->getTexture();
            }
            Tile& ShadowRenderComponent::getFrameBufferTile () {
                return *stencilBufferTile;
            }
            Tile& ShadowRenderComponent::getDepthBufferTile() {
                return *shadowTile;
            }
            void ShadowRenderComponent::setExpression(std::string expression) {
                update = true;
                this->expression = expression;
            }
            std::string ShadowRenderComponent::getExpression() {
                return expression;
            }
            void ShadowRenderComponent::setView(View view) {
                this->view = view;
                shadowMap->setView(view);
            }
            bool ShadowRenderComponent::loadEntitiesOnComponent(std::vector<Entity*> vEntities)
            {

                batcher.clear();
                shadowBatcher.clear();
                for (unsigned int i = 0; i < vEntities.size(); i++) {
                    if ( vEntities[i]->isLeaf()) {
                        Entity* entity = vEntities[i]->getParent();
                        math::Vec3f shadowOrigin, shadowCenter, shadowScale(1.f, 1.f, 1.f), shadowRotationAxis;
                        float shadowRotationAngle = 0;
                        if (entity->isModel()) {
                            shadowCenter = static_cast<Model*>(entity)->getShadowCenter();
                            shadowScale = static_cast<Model*>(entity)->getShadowScale();
                            shadowRotationAxis = static_cast<Model*>(entity)->getShadowRotationAxis();
                            shadowRotationAngle = static_cast<Model*>(entity)->getShadowRotationAngle();
                            shadowOrigin = static_cast<Model*>(entity)->getShadowOrigin();
                        }
                        TransformMatrix tm;
                        tm.setOrigin(entity->getPosition());
                        tm.setScale(shadowScale);
                        tm.setRotation(shadowRotationAxis, shadowRotationAngle);
                        tm.setTranslation(entity->getPosition() + shadowCenter);
                        for (unsigned int j = 0; j <  vEntities[i]->getFaces().size(); j++) {
                             batcher.addFace( vEntities[i]->getFaces()[j]);
                             shadowBatcher.addShadowFace(vEntities[i]->getFaces()[j], view.getViewMatrix(), tm);
                        }
                    }
                }
                m_instances = batcher.getInstances();
                m_shadow_instances = shadowBatcher.getInstances();
                visibleEntities = vEntities;
                update = true;
                return true;
            }
            void ShadowRenderComponent::clear() {
                 shadowMap->clear(sf::Color::White);
                 stencilBuffer->clear(sf::Color::Transparent);
            }
            void ShadowRenderComponent::changeVisibleEntities(Entity* toRemove, Entity* toAdd, EntityManager* em) {
                bool removed;
                em->removeAnimatedVisibleEntity(toRemove, visibleEntities, view, removed);
                if (removed) {
                    if (expression.find(toAdd->getRootType())) {
                        em->insertAnimatedVisibleEntity(toAdd, visibleEntities, view);
                    }
                    loadEntitiesOnComponent(visibleEntities);
                    update = true;
                }
            }
            void ShadowRenderComponent::updateParticleSystems() {
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
