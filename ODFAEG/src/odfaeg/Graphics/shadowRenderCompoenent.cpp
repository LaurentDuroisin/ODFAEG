#include "../../../include/odfaeg/Graphics/shadowRenderComponent.hpp"
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include "glCheck.h"
#include <memory.h>
using namespace sf;
using namespace std;
namespace odfaeg {
    namespace graphic {
        ShadowRenderComponent::ShadowRenderComponent (RenderWindow& window, int layer, std::string expression,window::ContextSettings settings) :
            HeavyComponent(window, math::Vec3f(window.getView().getPosition().x, window.getView().getPosition().y, layer),
                          math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0),
                          math::Vec3f(window.getView().getSize().x + window.getView().getSize().x * 0.5f, window.getView().getPosition().y + window.getView().getSize().y * 0.5f, layer)),
            view(window.getView()),
            expression(expression) {
                update = false;
                sf::Vector3i resolution ((int) window.getSize().x, (int) window.getSize().y, window.getView().getSize().z);
                shadowMap.create(resolution.x, resolution.y,settings);
                settings.depthBits = 24;
                stencilBuffer.create(resolution.x, resolution.y,settings);
                stencilBufferTile = Sprite(stencilBuffer.getTexture(), math::Vec3f(0, 0, 0), math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0), IntRect(0, 0, window.getView().getSize().x, window.getView().getSize().y));
                shadowTile = Sprite(shadowMap.getTexture(), math::Vec3f(0, 0, 0), math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0), IntRect(0, 0, window.getView().getSize().x, window.getView().getSize().y));
                core::FastDelegate<bool> signal (&ShadowRenderComponent::needToUpdate, this);
                core::FastDelegate<void> slot (&ShadowRenderComponent::drawNextFrame, this);
                core::Command cmd(signal, slot);
                getListener().connect("UPDATE", cmd);
                if (settings.versionMajor >= 3 && settings.versionMinor >= 3) {
                    glGenBuffers(1, &vboWorldMatrices);
                    const std::string buildShadowMapVertexShader = R"(#version 460 core
                                                        layout (location = 0) in vec3 position;
                                                        layout (location = 1) in vec4 color;
                                                        layout (location = 2) in vec2 texCoords;
                                                        layout (location = 3) in mat4 worldMat;
                                                        uniform mat4 projectionMatrix;
                                                        uniform mat4 viewMatrix;
                                                        uniform mat4 textureMatrix;
                                                        out vec2 fTexCoords;
                                                        out vec4 frontColor;
                                                        void main() {
                                                            gl_Position = projectionMatrix * viewMatrix  * worldMat * vec4(position, 1.f);
                                                            fTexCoords = (textureMatrix * vec4(texCoords, 1.f, 1.f)).xy;
                                                            frontColor = color;
                                                        }
                                                        )";
                    const std::string buildShadowMapFragmentShader = R"(#version 460 core
                                                                        in vec4 frontColor;
                                                                        in vec2 fTexCoords;
                                                                        uniform sampler2D texture;
                                                                        uniform float haveTexture;
                                                                        layout (location = 0) out vec4 fColor;
                                                                        void main() {
                                                                            vec4 texel = texture2D(texture, fTexCoords);
                                                                            vec4 colors[2];
                                                                            colors[1] = texel * frontColor;
                                                                            colors[0] = frontColor;
                                                                            bool b = (haveTexture >= 0.9f);
                                                                            float color = colors[int(b)].a;
                                                                            float z = gl_FragCoord.z;
                                                                            fColor = vec4(0, 0, z, color);
                                                                        }
                                                                    )";
                    const std::string perPixShadowVertexShader = R"(#version 460 core
                                                                   layout (location = 0) in vec3 position;
                                                                   layout (location = 1) in vec4 color;
                                                                   layout (location = 2) in vec2 texCoords;
                                                                   uniform mat4 projectionMatrix;
                                                                   uniform mat4 viewMatrix;
                                                                   uniform mat4 depthBiasMatrix;
                                                                   uniform mat4 textureMatrix;
                                                                   out vec4 shadowCoords;
                                                                   out vec4 frontColor;
                                                                   out vec2 fTexCoords;
                                                                   void main() {
                                                                       gl_Position = projectionMatrix * viewMatrix * vec4(position, 1.f);
                                                                       fTexCoords = (textureMatrix * vec4(texCoords, 1.f, 1.f)).xy;
                                                                       frontColor = color;
                                                                       shadowCoords = depthBiasMatrix * vec4(gl_Position.xyz, 1);
                                                                   }
                                                                  )";
                        const std::string perPixShadowFragmentShader = R"(#version 460 core
                                                                          in vec4 shadowCoords;
                                                                          in vec4 frontColor;
                                                                          in vec2 fTexCoords;
                                                                          uniform sampler2D texture;
                                                                          uniform sampler2D stencilBuffer;
                                                                          uniform float haveTexture;
                                                                          layout (location = 0) out vec4 fColor;
                                                                          void main() {
                                                                            vec4 texel = texture2D(texture, fTexCoords);
                                                                            vec4 colors[2];
                                                                            colors[1] = texel * frontColor;
                                                                            colors[0] = frontColor;
                                                                            bool b = (haveTexture >= 0.9f);
                                                                            float color = colors[int(b)].a;
                                                                            vec4 stencil = texture2D (stencilBuffer, shadowCoords.xy);
                                                                            float z = gl_FragCoord.z;
                                                                            colors[1] = vec4 (0, 0, 0, color);
                                                                            colors[0] = vec4 (0.5, 0.5, 0.5, 0.5);
                                                                            b = (stencil.z < z);
                                                                            vec4 visibility = colors[int(b)];
                                                                            fColor = visibility;
                                                                          }
                                                                          )";
                        if (!buildShadowMapShader.loadFromMemory(buildShadowMapVertexShader, buildShadowMapFragmentShader)) {
                            throw core::Erreur(53, "Error, failed to load build shadow map shader", 3);
                        }
                        if (!perPixShadowShader.loadFromMemory(perPixShadowVertexShader, perPixShadowFragmentShader)) {
                            throw core::Erreur(54, "Error, failed to load per pix shadow map shader", 3);
                        }
                        buildShadowMapShader.setParameter("texture", Shader::CurrentTexture);
                        perPixShadowShader.setParameter("stencilBuffer", stencilBuffer.getTexture());
                        perPixShadowShader.setParameter("texture", Shader::CurrentTexture);

                } else {
                    if (Shader::isAvailable()) {
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
                            "mat4 inverse(mat4 mat) {"
                            "   mat4 inv;"
                            "   return inv;"
                            "}"
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
                            "mat4 inverse(mat4 mat) {"
                            "   mat4 inv;"
                            "   return inv;"
                            "}"
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
                        if (!buildShadowMapShader.loadFromMemory(buildShadowMapVertexShader, buildShadowMapFragmentShader)) {
                            throw core::Erreur(53, "Error, failed to load build shadow map shader", 3);
                        }
                        if (!perPixShadowShader.loadFromMemory(perPixShadowVertexShader, perPixShadowFragmentShader)) {
                            throw core::Erreur(54, "Error, failed to load per pix shadow map shader", 3);
                        }
                        buildShadowMapShader.setParameter("texture", Shader::CurrentTexture);
                        perPixShadowShader.setParameter("stencilBuffer", stencilBuffer.getTexture());
                        perPixShadowShader.setParameter("texture", Shader::CurrentTexture);
                    }   else {
                        throw core::Erreur(55, "Shader not supported!", 0);
                    }
                }
            }
            void ShadowRenderComponent::drawNextFrame() {
                math::Vec3f centerLight = g2d::AmbientLight::getAmbientLight().getLightCenter();
                View lightView = View(view.getSize().x, view.getSize().y, -g2d::AmbientLight::getAmbientLight().getHeight(), g2d::AmbientLight::getAmbientLight().getHeight());
                lightView.setCenter(centerLight);
                math::Vec3f forward = view.getPosition() - lightView.getPosition();
                math::Vec3f target = lightView.getPosition() + forward;
                lightView.lookAt(target.x, target.y, target.z);
                stencilBuffer.setView(lightView);
                math::Vec3f v = lightView.getPosition() - view.getPosition();
                physic::BoundingBox viewArea = view.getViewVolume();
                math::Vec3f position (viewArea.getPosition().x,viewArea.getPosition().y, view.getPosition().z);
                math::Vec3f size (viewArea.getWidth(), viewArea.getHeight(), 0);
                if (shadowMap.getSettings().versionMajor >= 3 && shadowMap.getSettings().versionMinor >= 3) {
                    math::Matrix4f viewMatrix = lightView.getViewMatrix().getMatrix().transpose();
                    math::Matrix4f projMatrix = lightView.getProjMatrix().getMatrix().transpose();
                    buildShadowMapShader.setParameter("projectionMatrix", projMatrix);
                    buildShadowMapShader.setParameter("viewMatrix", viewMatrix);
                    RenderStates states;
                    states.shader = &buildShadowMapShader;
                    for (unsigned int i = 0; i < m_instances.size(); i++) {
                        if (m_instances[i].getAllVertices().getVertexCount() > 0) {
                            states.texture = m_instances[i].getMaterial().getTexture();
                            if (m_instances[i].getMaterial().getTexture() != nullptr) {
                                math::Matrix4f texMatrix = m_instances[i].getMaterial().getTexture()->getTextureMatrix();
                                buildShadowMapShader.setParameter("textureMatrix", texMatrix);
                                buildShadowMapShader.setParameter("haveTexture", 1.f);
                            } else {
                                buildShadowMapShader.setParameter("haveTexture", 0.f);
                            }
                            vb.clear();
                            vb.setPrimitiveType(m_instances[i].getVertexArrays()[0]->getPrimitiveType());
                            matrices.clear();
                            std::vector<TransformMatrix*> tm = m_instances[i].getTransforms();
                            for (unsigned int j = 0; j < tm.size(); j++) {
                                tm[j]->update();
                                std::array<float, 16> matrix = tm[j]->getMatrix().transpose().toGlMatrix();
                                for (unsigned int n = 0; n < 16; n++) {
                                    matrices.push_back(matrix[n]);
                                }
                            }
                            glCheck(glBindBuffer(GL_ARRAY_BUFFER, vboWorldMatrices));
                            glCheck(glBufferData(GL_ARRAY_BUFFER, matrices.size() * sizeof(float), &matrices[0], GL_DYNAMIC_DRAW));
                            glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
                            if (m_instances[i].getVertexArrays().size() > 0) {
                                for (unsigned int j = 0; j < m_instances[i].getVertexArrays()[0]->getVertexCount(); j++) {
                                    vb.append((*m_instances[i].getVertexArrays()[0])[j]);
                                }
                                vb.update();
                            }
                            stencilBuffer.drawInstanced(vb, vboWorldMatrices, m_instances[i].getVertexArrays()[0]->getPrimitiveType(), 0, m_instances[i].getVertexArrays()[0]->getVertexCount(), tm.size(), states);
                        }
                    }
                    stencilBuffer.display();
                    stencilBufferTile.setPosition(position);
                    shadowMap.setView(view);
                    math::Matrix4f biasMatrix(0.5f, 0.0f, 0.0f, 0.0f,
                                              0.0f, 0.5f, 0.0f, 0.0f,
                                              0.0f, 0.0f, 0.5f, 0.0f,
                                              0.5f, 0.5f, 0.5f, 1.f);
                    math::Matrix4f depthBiasMatrix = biasMatrix * view.getViewMatrix().getMatrix() * view.getProjMatrix().getMatrix();
                    perPixShadowShader.setParameter("depthBiasMatrix", depthBiasMatrix.transpose());
                    projMatrix = view.getProjMatrix().getMatrix().transpose();
                    perPixShadowShader.setParameter("projectionMatrix", projMatrix);
                    viewMatrix = view.getViewMatrix().getMatrix().transpose();
                    perPixShadowShader.setParameter("viewMatrix", viewMatrix);
                    states.shader = &perPixShadowShader;
                    for (unsigned int i = 0; i < m_shadow_instances.size(); i++) {
                        if (m_shadow_instances[i].getAllVertices().getVertexCount() > 0) {
                            states.texture = m_shadow_instances[i].getMaterial().getTexture();
                            if (m_shadow_instances[i].getMaterial().getTexture() != nullptr) {
                                math::Matrix4f texMatrix = m_shadow_instances[i].getMaterial().getTexture()->getTextureMatrix();
                                perPixShadowShader.setParameter("textureMatrix", texMatrix);
                                perPixShadowShader.setParameter("haveTexture", 1.f);
                            } else {
                                perPixShadowShader.setParameter("haveTexture", 0.f);
                            }
                            vb.clear();
                            vb.setPrimitiveType(m_shadow_instances[i].getAllVertices().getPrimitiveType());
                            for (unsigned int j = 0; j < m_shadow_instances[i].getAllVertices().getVertexCount(); j++) {
                                vb.append(m_shadow_instances[i].getAllVertices()[j]);
                            }
                            vb.update();
                            shadowMap.drawVertexBuffer(vb, states);
                        }
                    }
                    shadowMap.display();
                } else {
                    RenderStates states;
                    states.shader = &buildShadowMapShader;
                    for (unsigned int i = 0; i < m_instances.size(); i++) {
                        if (m_instances[i].getAllVertices().getVertexCount() > 0) {
                            states.texture = m_instances[i].getMaterial().getTexture();
                            if (m_instances[i].getMaterial().getTexture() != nullptr) {
                                buildShadowMapShader.setParameter("haveTexture", 1.f);
                            } else {
                                buildShadowMapShader.setParameter("haveTexture", 0.f);
                            }
                            stencilBuffer.draw(m_instances[i].getAllVertices(), states);
                        }
                    }
                    stencilBuffer.display();
                    stencilBufferTile.setPosition(position);
                    shadowMap.setView(view);
                    math::Matrix4f biasMatrix(0.5f, 0.0f, 0.0f, 0.0f,
                                              0.0f, 0.5f, 0.0f, 0.0f,
                                              0.0f, 0.0f, 0.5f, 0.0f,
                                              0.5f, 0.5f, 0.5f, 1.f);
                    math::Matrix4f depthBiasMatrix = biasMatrix * view.getViewMatrix().getMatrix() * view.getProjMatrix().getMatrix();
                    perPixShadowShader.setParameter("depthBiasMatrix", depthBiasMatrix.transpose());
                    states.shader = &perPixShadowShader;
                    for (unsigned int i = 0; i < m_shadow_instances.size(); i++) {
                        if (m_shadow_instances[i].getAllVertices().getVertexCount() > 0) {
                            states.texture = m_shadow_instances[i].getMaterial().getTexture();
                            if (m_shadow_instances[i].getMaterial().getTexture() != nullptr) {
                                perPixShadowShader.setParameter("haveTexture", 1.f);
                            } else {
                                perPixShadowShader.setParameter("haveTexture", 0.f);
                            }

                            shadowMap.draw(m_shadow_instances[i].getAllVertices(), states);
                        }
                    }
                    shadowMap.display();
                }
            }
            std::vector<Entity*> ShadowRenderComponent::getEntities() {
                return visibleEntities;
            }
            void ShadowRenderComponent::draw(RenderTarget& target, RenderStates states) {
                shadowTile.setCenter(target.getView().getPosition());
                states.blendMode = sf::BlendMultiply;
                target.draw(shadowTile, states);
            }
            void ShadowRenderComponent::pushEvent(window::IEvent event, RenderWindow& rw) {
                if (event.type == window::IEvent::WINDOW_EVENT && event.window.type == window::IEvent::WINDOW_EVENT_RESIZED && &getWindow() == &rw && isAutoResized()) {
                    recomputeSize();
                    getListener().pushEvent(event);
                    getView().reset(physic::BoundingBox(getView().getViewport().getPosition().x, getView().getViewport().getPosition().y, getView().getViewport().getPosition().z, event.window.data1, event.window.data2, getView().getViewport().getDepth()));
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
                return stencilBuffer.getTexture();
            }
            const Texture& ShadowRenderComponent::getShadowMapTexture() {
                return shadowMap.getTexture();
            }
            Sprite& ShadowRenderComponent::getFrameBufferTile () {
                return stencilBufferTile;
            }
            Sprite& ShadowRenderComponent::getDepthBufferTile() {
                return shadowTile;
            }
            void ShadowRenderComponent::setExpression(std::string expression) {
                update = true;
                this->expression = expression;
            }
            std::string ShadowRenderComponent::getExpression() {
                return expression;
            }
            void ShadowRenderComponent::setView(View view) {
                this->view = view;/*View(view.getSize().x, view.getSize().y, view.getPosition().z, view.getDepth());
                this->view.setCenter(view.getPosition());*/
                shadowMap.setView(this->view);
            }
            bool ShadowRenderComponent::loadEntitiesOnComponent(std::vector<Entity*> vEntities)
            {

                batcher.clear();
                shadowBatcher.clear();
                for (unsigned int i = 0; i < vEntities.size(); i++) {
                    if ( vEntities[i]->isLeaf()) {
                        Entity* entity = vEntities[i]->getRootEntity();
                        math::Vec3f shadowOrigin, shadowCenter, shadowScale(1.f, 1.f, 1.f), shadowRotationAxis, shadowTranslation;
                        float shadowRotationAngle = 0;
                        //if (entity != nullptr && entity->isModel()) {
                            shadowCenter = entity->getShadowCenter();
                            shadowScale = entity->getShadowScale();
                            shadowRotationAxis = entity->getShadowRotationAxis();
                            shadowRotationAngle = entity->getShadowRotationAngle();
                            shadowOrigin = entity->getPosition();
                            shadowTranslation = entity->getPosition() + shadowCenter;
                        //}
                        TransformMatrix tm;
                        tm.setOrigin(shadowOrigin);
                        tm.setScale(shadowScale);
                        tm.setRotation(shadowRotationAxis, shadowRotationAngle);
                        tm.setTranslation(shadowTranslation);
                        for (unsigned int j = 0; j <  vEntities[i]->getNbFaces(); j++) {
                             batcher.addFace( vEntities[i]->getFace(j));
                             shadowBatcher.addShadowFace(vEntities[i]->getFace(j), view.getViewMatrix(), tm);
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
                 shadowMap.clear(sf::Color::White);
                 stencilBuffer.clear(sf::Color::Transparent);
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
