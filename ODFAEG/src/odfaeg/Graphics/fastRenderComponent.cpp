#include "../../../include/odfaeg/Graphics/fastRenderComponent.hpp"
#include "../../../include/odfaeg/Math/triangle.h"
#include "glCheck.h"
#include <memory.h>
using namespace sf;
using namespace std;
namespace odfaeg {
    namespace graphic {
        FastRenderComponent::FastRenderComponent (RenderWindow& window, int layer, std::string expression, bool useThread) :
            HeavyComponent(math::Vec3f(window.getView().getPosition().x, window.getView().getPosition().y, layer),
                          math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0),
                          math::Vec3f(window.getView().getSize().x + window.getView().getSize().x * 0.5f, window.getView().getPosition().y + window.getView().getSize().y * 0.5f, layer), useThread),
            view(window.getView()),
            expression(expression),
            rw(window) {
            update = false;
            sf::Vector3i resolution ((int) window.getSize().x, (int) window.getSize().y, window.getView().getSize().z);
            depthBuffer = new RenderTexture();
            frameBuffer = new RenderTexture();
            specularTexture = new RenderTexture();
            bumpTexture = new RenderTexture();
            refractionTexture = new RenderTexture();
            depthBuffer->create(resolution.x, resolution.y,window.getSettings(), true);
            ContextSettings settings = window.getSettings();
            settings.depthBits = 24;
            frameBuffer->create(resolution.x, resolution.y,settings);
            specularTexture->create(resolution.x, resolution.y,window.getSettings());
            bumpTexture->create(resolution.x, resolution.y,window.getSettings());
            refractionTexture->create(resolution.x, resolution.y,window.getSettings());
            frameBuffer->setView(window.getView());
            depthBuffer->setView(window.getView());
            specularTexture->setView(window.getView());
            bumpTexture->setView(window.getView());
            refractionTexture->setView(window.getView());
            frameBuffer->clear(sf::Color::Transparent);
            depthBuffer->clear(sf::Color::Transparent);
            specularTexture->clear(sf::Color::Transparent);
            bumpTexture->clear(sf::Color::Transparent);
            frameBufferTile = new Tile(&frameBuffer->getTexture(), math::Vec3f(0, 0, 0), math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0), IntRect(0, 0, window.getView().getSize().x, window.getView().getSize().y));
            depthBufferTile = new Tile(&depthBuffer->getTexture(), math::Vec3f(0, 0, 0), math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0), IntRect(0, 0, window.getView().getSize().x, window.getView().getSize().y));
            if (Shader::isAvailable()) {
                frameBufferGenerator = new Shader();
                depthBufferGenerator = new Shader();
                specularTextureGenerator = new Shader();
                bumpTextureGenerator = new Shader();
                refractionTextureGenerator = new Shader();
                simpleShader = new Shader();
                //With modern openg, we need to use another vertex shader.
                //GLSL 3 is not supported by all the drivers so we use GLSL 1.
                if (Shader::getShadingLanguageVersionMajor() >= 3 && Shader::getShadingLanguageVersionMinor() >= 3) {
                    const std::string simpleVertexShader =
                    "#version 330 core\n"
                    "layout (location = 0) in vec4 position;"
                    "layout (location = 1) in vec4 color;"
                    "layout (location = 2) in vec2 uv;"
                    "layout (location = 3) in vec3 normals;"
                    "uniform mat4 p;"
                    "uniform mat4 v;"
                    "uniform mat4 m;"
                    "uniform mat4 t;"
                    "uniform float haveTexture;"
                    "out vec4 f_color;"
                    "out vec2 texCoords;"
                    "void main() {"
                    "   gl_Position = p * v * m * position;"
                    "   texCoords = (haveTexture == 1) ? (t * vec4(uv.xy, 0, 0)).xy : vec2(0, 0);"
                    "   f_color = color;"
                    "}";
                    const std::string vertexShader =
                    "#version 330 core\n"
                    "layout (location = 0) in vec4 position;"
                    "layout (location = 1) in vec4 color;"
                    "layout (location = 2) in vec2 uv;"
                    "layout (location = 3) in vec3 normals;"
                    "uniform mat4 p;"
                    "uniform mat4 v;"
                    "uniform mat4 m;"
                    "uniform mat4 t;"
                    "uniform float haveTexture;"
                    "out vec4 f_color;"
                    "out vec2 texCoords;"
                    "out mat4 projMat;"
                    "void main() {"
                    "   gl_Position = p * v * m * position;"
                    "   texCoords = (haveTexture == 1) ? (t * vec4(uv.xy, 0, 0)).xy : vec2(0, 0);"
                    "   f_color = color;"
                    "   projMat = p;"
                    "}";
                    const std::string simpleFragmentShader =
                    "#version 330 core\n"
                    "uniform sampler2D texSampler;"
                    "uniform float haveTexture;"
                    "in vec2 texCoords;"
                    "in vec4 f_color;"
                    "void main() {"
                    "   gl_FragColor = (haveTexture == 1) ? f_color * texture (texSampler, texCoords) : f_color;"
                    "}";
                    if (!simpleShader->loadFromMemory(simpleVertexShader, simpleFragmentShader))
                        throw core::Erreur(49, "Failed to load simple shader", 0);
                    simpleShader->setParameter("texSampler", Shader::CurrentTexture);
                } else {
                   const std::string  vertexShader =
                   "#version 130 \n"
                   "out mat4 projMat;"
                   "void main () {"
                        "gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
                        "gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;"
                        "gl_FrontColor = gl_Color;"
                        "projMat = gl_ProjectionMatrix;"
                   "}";
                   const std::string specularGenFragShader =
                        "#version 130 \n"
                        "uniform sampler2D specularTexture;"
                        "uniform sampler2D texture;"
                        "uniform vec3 resolution;"
                        "uniform float maxM;"
                        "uniform float maxP;"
                        "uniform float m;"
                        "uniform float p;"
                        "uniform float haveTexture;"
                        "in mat4 projMat;"
                        "void main() {"
                            "vec2 position = ( gl_FragCoord.xy / resolution.xy );"
                            "vec4 color = texture2D(specularTexture, position);"
                            "vec4 pixel = (haveTexture==1) ? gl_Color * texture2D(texture, gl_TexCoord[0].xy) : gl_Color;"
                            "float z = (gl_FragCoord.w != 1.f) ? (inverse(projMat) * vec4(0, 0, 0, gl_FragCoord.w)).w : gl_FragCoord.z;"
                            "if(z >= color.z && pixel.a >= color.a) {"
                                "float intensity = (maxM != 0.f) ? m / maxM : 0.f;"
                                "float power = (maxP != 0.f) ? p / maxP : 0.f;"
                                "gl_FragColor = vec4(intensity, power, z, pixel.a);"
                            "} else {"
                                "gl_FragColor = color;"
                            "}"
                        "}";
                        const std::string bumpGenFragShader =
                        "#version 130 \n"
                        "uniform sampler2D bumpTexture;"
                        "uniform sampler2D texture;"
                        "uniform vec3 resolution;"
                        "uniform float haveTexture;"
                        "in mat4 projMat;"
                        "void main() {"
                            "vec2 position = ( gl_FragCoord.xy / resolution.xy );"
                            "vec4 bump1 = texture2D(bumpTexture, position);"
                            "vec4 bump2 = (haveTexture==1) ? texture2D(texture, gl_TexCoord[0].xy) : vec4(0, 0, 0, 0);"
                            "float z = (gl_FragCoord.w != 1.f) ? (inverse(projMat) * vec4(0, 0, 0, gl_FragCoord.w)).w : gl_FragCoord.z;"
                            "if (z >= bump1.a) {"
                                "gl_FragColor = vec4(bump2.xyz, z);"
                            "} else {"
                                "gl_FragColor = bump1;"
                            "}"
                        "}";
                        const std::string refractionGenFragShader =
                        "#version 130 \n"
                        "uniform sampler2D refractionTexture;"
                        "uniform vec3 resolution;"
                        "uniform float refractionFactor;"
                        "in mat4 projMat;"
                        "void main() {"
                            "vec2 position = ( gl_FragCoord.xy / resolution.xy );"
                            "vec4 refraction = texture2D(refractionTexture, position);"
                            "float z = (gl_FragCoord.w != 1.f) ? (inverse(projMat) * vec4(0, 0, 0, gl_FragCoord.w)).w : gl_FragCoord.z;"
                            "if (z >= refraction.a) {"
                                "gl_FragColor = vec4(refractionFactor, 0, z, 0);"
                            "} else {"
                                "gl_FragColor = refraction;"
                            "}"
                        "}";
                        if (!specularTextureGenerator->loadFromMemory(vertexShader, specularGenFragShader))
                            throw core::Erreur(52, "Failed to load specular texture generator shader", 0);
                        if (!bumpTextureGenerator->loadFromMemory(vertexShader, bumpGenFragShader))
                            throw core::Erreur(53, "Failed to load bump texture generator shader", 0);
                        if (!refractionTextureGenerator->loadFromMemory(vertexShader, refractionGenFragShader))
                            throw core::Erreur(54, "Failed to load refraction texture generator shader", 0);
                }
            }
            core::FastDelegate<bool> signal (&FastRenderComponent::needToUpdate, this);
            core::FastDelegate<void> slot (&FastRenderComponent::drawNextFrame, this);
            core::Command cmd(signal, slot);
            getListener().connect("UPDATE", cmd);
            mvpBuffer = 0;
            backgroundColor = sf::Color::Transparent;
        }
        void FastRenderComponent::pushEvent(sf::Event event) {
            getListener().pushEvent(event);
        }
        bool FastRenderComponent::needToUpdate() {
            return update;
        }
        void FastRenderComponent::changeVisibleEntities(Entity* toRemove, Entity* toAdd, EntityManager* em) {
            bool removed;
            em->removeAnimatedVisibleEntity(toRemove, visibleEntities, view, removed);
            if (removed) {
                em->insertAnimatedVisibleEntity(toAdd, visibleEntities, view);
                loadEntitiesOnComponent(visibleEntities);
                update = true;
            }
        }
        std::string FastRenderComponent::getExpression() {
            return expression;
        }
        void FastRenderComponent::setBackgroundColor(sf::Color color) {
            this->backgroundColor = color;
        }
        void FastRenderComponent::clear() {
             frameBuffer->clear(backgroundColor);
             depthBuffer->clear(sf::Color::Transparent);
             specularTexture->clear(sf::Color::Transparent);
             bumpTexture->clear(sf::Color::Transparent);
             refractionTexture->clear(sf::Color::Transparent);
        }
        Tile& FastRenderComponent::getFrameBufferTile () {
            return *frameBufferTile;
        }
        Tile& FastRenderComponent::getDepthBufferTile() {
            return *depthBufferTile;
        }
        const Texture& FastRenderComponent::getDepthBufferTexture() {
            return depthBuffer->getTexture();
        }
        const Texture& FastRenderComponent::getFrameBufferTexture() {
            return frameBuffer->getTexture();
        }
        const Texture& FastRenderComponent::getSpecularTexture() {
            return specularTexture->getTexture();
        }
        bool FastRenderComponent::loadEntitiesOnComponent(std::vector<Entity*> vEntities)
        {
            sortedVisibleEntities.clear();
            View winview = rw.getView();
            rw.setView(view);
            for (unsigned int i = 0; i < vEntities.size(); i++) {
                math::Vec3f screenCoords = rw.mapCoordsToPixel(vEntities[i]->getPosition());
                sortedVisibleEntities.insert(std::make_pair(screenCoords.z, vEntities[i]));
            }
            rw.setView(winview);
            batcher.clear();
            std::multimap<float, Entity*>::iterator it;
            for (it = sortedVisibleEntities.begin(); it != sortedVisibleEntities.end(); it++) {
                 if ( it->second->isLeaf()) {
                    for (unsigned int j = 0; j <  it->second->getFaces().size(); j++) {
                        batcher.addFace(it->second->getFaces()[j]);
                    }
                }
            }
            m_instances = batcher.getInstances();
            visibleEntities = vEntities;
            update = true;
            return true;
        }
        void FastRenderComponent::setView(View view) {
            this->view = view;
            frameBuffer->setView(view);
            depthBuffer->setView(view);
            specularTexture->setView(view);
            bumpTexture->setView(view);
            refractionTexture->setView(view);
        }
        void FastRenderComponent::setExpression(std::string expression) {
            update = true;
            this->expression = expression;
        }
        void FastRenderComponent::drawNextFrame() {
            update = false;
            if (Shader::isAvailable()) {
                if (Shader::getShadingLanguageVersionMajor() >= 4 && Shader::getShadingLanguageVersionMinor() >= 2) {
                    /*math::Matrix4f vmatrix = view.getViewMatrix().getMatrix().transpose();
                    math::Matrix4f pmatrix = view.getProjMatrix().getMatrix().transpose();
                    frameBufferGenerator->setParameter("v", vmatrix);
                    frameBufferGenerator->setParameter("p", pmatrix);
                    depthBufferGenerator->setParameter("v", vmatrix);
                    depthBufferGenerator->setParameter("p", pmatrix);

                    for (unsigned int i = 0; i < m_instances.size(); i++) {
                        std::vector<float> instancesMVP;
                        for (unsigned int j = 0; j < m_instances[i]->getTransforms().size(); j++) {
                            math::Matrix4f mmatrix = m_instances[i]->getTransforms()[j].get().getMatrix().transpose();
                            float* glMatrix = mmatrix.toGlMatrix();
                            for (unsigned int i = 0; i < 16; i++)
                                instancesMVP.push_back(glMatrix[i]);
                        }
                        if (mvpBuffer == 0) {
                            GLuint vboMVP;
                            glCheck(glGenBuffers(1, &vboMVP));
                            mvpBuffer = reinterpret_cast<unsigned int>(vboMVP);
                            glCheck(glBindBuffer(GL_ARRAY_BUFFER, mvpBuffer));
                            glCheck(glBufferDataARB(GL_ARRAY_BUFFER, instancesMVP.size() * sizeof(math::Vec3f),&instancesMVP[0], GL_DYNAMIC_DRAW));
                            glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
                        } else {
                            GLvoid *pos_vbo = nullptr;
                            glCheck(glBindBuffer(GL_ARRAY_BUFFER, mvpBuffer));
                            pos_vbo = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
                            if (pos_vbo != nullptr) {
                                memcpy(pos_vbo,&instancesMVP[0], instancesMVP.size() * sizeof(math::Vec3f));
                                glCheck(glUnmapBuffer(GL_ARRAY_BUFFER));
                                pos_vbo = nullptr;
                            }
                            glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
                        }
                        currentStates.mvpBufferId = mvpBuffer;
                        currentStates.texture = m_instances[i]->getMaterial().getTexture();
                        currentStates.shader = frameBufferGenerator;
                        currentStates.numInstances = m_instances[i]->getNumInstances();
                        frameBuffer->draw(m_instances[i]->getVertexArray(), currentStates);
                        frameBuffer->display();
                        currentStates.shader = depthBufferGenerator;
                        depthBuffer->draw(m_instances[i]->getVertexArray(), currentStates);
                        depthBuffer->display();
                    }*/
                } else if (Shader::getShadingLanguageVersionMajor() >= 3 && Shader::getShadingLanguageVersionMinor() >= 3) {
                    math::Matrix4f vmatrix = view.getViewMatrix().getMatrix().transpose();
                    math::Matrix4f pmatrix = view.getProjMatrix().getMatrix().transpose();
                    frameBufferGenerator->setParameter("v", vmatrix);
                    frameBufferGenerator->setParameter("p", pmatrix);
                    depthBufferGenerator->setParameter("v", vmatrix);
                    depthBufferGenerator->setParameter("p", pmatrix);
                    simpleShader->setParameter("v", vmatrix);
                    simpleShader->setParameter("p", pmatrix);
                    for (unsigned int i = 0; i < m_instances.size(); i++) {
                        currentStates.texture = m_instances[i]->getMaterial().getTexture();
                        if (currentStates.texture != nullptr) {
                            math::Matrix4f tmatrix = m_instances[i]->getMaterial().getTexture()->getTextureMatrix().transpose();
                            frameBufferGenerator->setParameter("t", tmatrix);
                            depthBufferGenerator->setParameter("t", tmatrix);
                            frameBufferGenerator->setParameter("haveTexture", 1.f);
                            depthBufferGenerator->setParameter("haveTexture", 1.f);
                        } else {
                            math::Matrix4f tmatrix;
                            tmatrix.zero();
                            frameBufferGenerator->setParameter("t", tmatrix);
                            depthBufferGenerator->setParameter("t", tmatrix);
                            frameBufferGenerator->setParameter("haveTexture", 0.f);
                            depthBufferGenerator->setParameter("haveTexture", 0.f);
                        }
                        for (unsigned int j = 0; j < m_instances[i]->getVertexArrays().size(); j++) {
                            math::Matrix4f mmatrix = m_instances[i]->getTransforms()[j].get().getMatrix().transpose();
                            frameBufferGenerator->setParameter("m", mmatrix);
                            depthBufferGenerator->setParameter("m", mmatrix);
                            currentStates.shader = frameBufferGenerator;
                            frameBuffer->draw(*m_instances[i]->getVertexArrays()[j], currentStates);
                            currentStates.shader = depthBufferGenerator;
                            depthBuffer->draw(*m_instances[i]->getVertexArrays()[j], currentStates);
                        }
                        for (unsigned int i = 0; i < drawables.size(); i++) {
                            if (drawables[i].second.texture != nullptr) {
                                math::Matrix4f tmatrix = m_instances[i]->getMaterial().getTexture()->getTextureMatrix().transpose();
                                simpleShader->setParameter("t", tmatrix);
                                simpleShader->setParameter("haveTexture", 1.f);
                            } else {
                                math::Matrix4f tmatrix;
                                tmatrix.zero();
                                simpleShader->setParameter("t", tmatrix);
                                simpleShader->setParameter("haveTexture", 0.f);
                            }
                            math::Matrix4f mmatrix = drawables[i].second.transform.getMatrix().transpose();
                            simpleShader->setParameter("m", mmatrix);
                            drawables[i].second.shader = simpleShader;
                            frameBuffer->draw(drawables[i].first.get(), drawables[i].second);
                        }
                        depthBuffer->display();
                        frameBuffer->display();
                    }
                } else {
                    for (unsigned int i = 0; i < m_instances.size(); i++) {
                        float specularIntensity = m_instances[i]->getMaterial().getSpecularIntensity();
                        float specularPower = m_instances[i]->getMaterial().getSpecularPower();
                        float refractionFactor = m_instances[i]->getMaterial().getRefractionFactor();
                        specularTextureGenerator->setParameter("m", specularIntensity);
                        specularTextureGenerator->setParameter("p", specularPower);
                        refractionTextureGenerator->setParameter("refractionFactor", refractionFactor);
                        if (m_instances[i]->getMaterial().getTexture() != nullptr) {
                            specularTextureGenerator->setParameter("haveTexture", 1.f);
                        } else {
                            specularTextureGenerator->setParameter("haveTexture", 0.f);
                        }
                        if (m_instances[i]->getMaterial().getBumpTexture() != nullptr) {
                            bumpTextureGenerator->setParameter("haveTexture", 1.f);
                        } else {
                            bumpTextureGenerator->setParameter("haveTexture", 0.f);
                        }
                        VertexArray va = m_instances[i]->getVertexArray();
                        currentStates.texture = m_instances[i]->getMaterial().getTexture();
                        currentStates.shader = nullptr;
                        currentStates.blendMode = sf::BlendAlpha;
                        frameBuffer->draw(va, currentStates);
                        currentStates.blendMode = sf::BlendNone;
                        depthBuffer->draw(va, currentStates);
                        currentStates.shader = bumpTextureGenerator;
                        currentStates.texture = m_instances[i]->getMaterial().getBumpTexture();
                        bumpTexture->draw(va, currentStates);
                        currentStates.shader = refractionTextureGenerator;
                        refractionTexture->draw(va, currentStates);
                    }
                    for (unsigned int i = 0; i < drawables.size(); i++) {
                        frameBuffer->draw(drawables[i].first.get(), drawables[i].second);
                    }
                    depthBuffer->display();
                    frameBuffer->display();
                    specularTexture->display();
                    bumpTexture->display();
                    refractionTexture->display();
                }
            } else {
                for (unsigned int i = 0; i < visibleEntities.size(); i++) {
                    for (unsigned int j = 0; j < visibleEntities[i]->getFaces().size(); j++) {
                        currentStates.texture = visibleEntities[i]->getFaces()[j]->getMaterial().getTexture();
                        frameBuffer->draw(visibleEntities[i]->getFaces()[j]->getVertexArray(), currentStates);
                    }
                }
            }
        }
        std::vector<Entity*> FastRenderComponent::getEntities() {
            return visibleEntities;
        }
        void FastRenderComponent::draw(Drawable& drawable, RenderStates states) {
            update = true;
            drawables.push_back(std::make_pair(std::ref(drawable), states));
        }
        void FastRenderComponent::draw(RenderTarget& target, RenderStates states) {
            if (Shader::getShadingLanguageVersionMajor() >= 3 && Shader::getShadingLanguageVersionMinor() >= 3) {
                frameBufferTile->setCenter(target.getView().getPosition());
                frameBufferTile->setScale(math::Vec3f(1.f, -1.f, 1.f));
                math::Matrix4f pmatrix = target.getView().getProjMatrix().getMatrix().transpose();
                math::Matrix4f vmatrix = target.getView().getViewMatrix().getMatrix().transpose();
                math::Matrix4f mmatrix = frameBufferTile->getTransform().getMatrix().transpose();
                math::Matrix4f tmatrix = frameBuffer->getTexture().getTextureMatrix().transpose();
                simpleShader->setParameter("p", pmatrix);
                simpleShader->setParameter("v", vmatrix);
                simpleShader->setParameter("m", mmatrix);
                simpleShader->setParameter("t", tmatrix);
                simpleShader->setParameter("haveTexture", 1.f);
                states.shader = simpleShader;
                states.transform = getTransform();
                target.draw(*frameBufferTile, states);
                drawables.clear();
            } else {
                frameBufferTile->setCenter(target.getView().getPosition());
                target.draw(*frameBufferTile, states);
                drawables.clear();
            }
        }

        View& FastRenderComponent::getView() {
            return view;
        }
        int FastRenderComponent::getLayer() {
            return getPosition().z;
        }
        const Texture& FastRenderComponent::getBumpTexture() {
            return bumpTexture->getTexture();
        }
        const Texture& FastRenderComponent::getRefractionTexture() {
            return refractionTexture->getTexture();
        }
        FastRenderComponent::~FastRenderComponent() {
            delete frameBuffer;
            delete frameBufferGenerator;
            delete depthBuffer;
            delete depthBufferGenerator;
            delete frameBufferTile;
            delete depthBufferTile;
            delete specularTexture;
            delete specularTextureGenerator;
            delete bumpTexture;
            delete bumpTextureGenerator;
            delete refractionTexture;
            delete refractionTextureGenerator;
            delete simpleShader;
            GLuint mvp = reinterpret_cast<GLuint>(mvpBuffer);
            glCheck(glDeleteBuffers(1, &mvp));
        }
    }
}
