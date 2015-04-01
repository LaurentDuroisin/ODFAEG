#include "../../../include/odfaeg/Graphics/fastRenderComponent.h"
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
            expression(expression) {
            update = false;
            sf::Vector3i resolution ((int) window.getSize().x, (int) window.getSize().y, window.getView().getSize().z);
            depthBuffer = new RenderTexture();
            frameBuffer = new RenderTexture();
            specularTexture = new RenderTexture();
            bumpTexture = new RenderTexture();
            refractionTexture = new RenderTexture();
            depthBuffer->create(resolution.x, resolution.y,window.getSettings());
            frameBuffer->create(resolution.x, resolution.y,window.getSettings());
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
            refractionTexture->clear(sf::Color::Transparent);
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
                    const std::string depthGenFragShader =
                    "#version 330 core \n"
                    "layout(origin_upper_left, pixel_center_integer) in vec4 gl_FragCoord;"
                    "uniform sampler2D depthBuffer;"
                    "uniform sampler2D texSampler;"
                    "uniform vec3 resolution;"
                    "uniform float haveTexture;"
                    "in vec2 texCoords;"
                    "in vec4 f_color;"
                    "in mat4 projMat;"
                    "void main () {"
                          "vec2 position = ( gl_FragCoord.xy / resolution.xy );"
                          "vec4 previous_depth_alpha = texture(depthBuffer, position);"
                          "vec4 texel = texture(texSampler, texCoords.xy);"
                          "vec4 colors[2];"
                          "colors[1] = texel * f_color;"
                          "colors[0] = f_color;"
                          "bool b = (haveTexture == 1);"
                          "float current_alpha = colors[int(b)].a;"
                          "float current_depth = (gl_FragCoord.w != 1.f) ? (inverse(projMat) * vec4(0, 0, 0, gl_FragCoord.w)).w : gl_FragCoord.z;"
                          "colors[1] = vec4(current_depth, current_alpha, current_depth, current_alpha);"
                          "colors[0] = vec4(current_depth, current_alpha, previous_depth_alpha.z, previous_depth_alpha.a);"
                          "b = (current_depth >= previous_depth_alpha.z && current_alpha != 0);"
                          "gl_FragColor = colors[int(b)];"
                    "}";
                    const std::string frameBufferGenFragShader =
                    "#version 330 core \n"
                    "layout(origin_upper_left, pixel_center_integer) in vec4 gl_FragCoord;"
                    "uniform sampler2D depthBuffer;"
                    "uniform sampler2D frameBuffer;"
                    "uniform sampler2D texSampler;"
                    "uniform vec3 resolution;"
                    "uniform float haveTexture;"
                    "in vec2 texCoords;"
                    "in vec4 f_color;"
                    "in mat4 projMat;"
                    "void main () {"
                        "vec2 position = ( gl_FragCoord.xy / resolution.xy );"
                        "vec4 previous_depth_alpha = texture(depthBuffer, position);"
                        "vec4 previous_color = texture(frameBuffer, position);"
                        "vec4 texel = texture(texSampler, texCoords.xy);"
                        "vec4 colors[2];"
                        "colors[1] = texel * f_color;"
                        "colors[0] = f_color;"
                        "bool b = (haveTexture == 1);"
                        "vec4 current_color = colors[int(b)];"
                        "float current_depth = (gl_FragCoord.w != 1.f) ? (inverse(projMat) * vec4(0, 0, 0, gl_FragCoord.w)).w : gl_FragCoord.z;"
                        "colors[1] = current_color * current_color.a + previous_color * (1 - current_color.a);"
                        "colors[1].a = current_color.a + previous_color.a * (1 - current_color.a);"
                        "colors[0] = previous_color * previous_depth_alpha.a + current_color * (1 - previous_depth_alpha.a);"
                        "colors[0].a = previous_color.a + previous_depth_alpha.a * (1 - previous_color.a);"
                        "b = (current_depth >= previous_depth_alpha.z);"
                        "gl_FragColor = colors[int(b)];"
                    "}";
                    if (!simpleShader->loadFromMemory(simpleVertexShader, simpleFragmentShader))
                        throw core::Erreur(49, "Failed to load simple shader", 0);
                    if (!depthBufferGenerator->loadFromMemory(vertexShader, depthGenFragShader))
                        throw core::Erreur(50, "Failed to load depth buffer generator shader", 0);
                    if (!frameBufferGenerator->loadFromMemory(vertexShader, frameBufferGenFragShader))
                        throw core::Erreur(51, "Failed to load frame buffer generator shader", 0);
                    simpleShader->setParameter("texSampler", Shader::CurrentTexture);
                    frameBufferGenerator->setParameter("resolution",resolution.x, resolution.y, resolution.z);
                    frameBufferGenerator->setParameter("depthBuffer", depthBuffer->getTexture());
                    frameBufferGenerator->setParameter("frameBuffer", frameBuffer->getTexture());
                    frameBufferGenerator->setParameter("texSampler", Shader::CurrentTexture);
                    depthBufferGenerator->setParameter("resolution",resolution.x, resolution.y, resolution.z);
                    depthBufferGenerator->setParameter("depthBuffer", depthBuffer->getTexture());
                    depthBufferGenerator->setParameter("texSampler", Shader::CurrentTexture);
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
                         const std::string depthGenFragShader =
                        "#version 130 \n"
                        "uniform sampler2D depthBuffer;"
                        "uniform sampler2D texture;"
                        "uniform vec3 resolution;"
                        "uniform float haveTexture;"
                        "in mat4 projMat;"
                        "void main () {"
                            "vec2 position = ( gl_FragCoord.xy / resolution.xy );"
                            "vec4 previous_depth_alpha = texture2D(depthBuffer, position);"
                            "vec4 texel = texture2D(texture, gl_TexCoord[0].xy);"
                            "vec4 colors[2];"
                            "colors[1] = texel * gl_Color;"
                            "colors[0] = gl_Color;"
                            "bool b = (haveTexture == 1);"
                            "float current_alpha = colors[int(b)].a;"
                            "float current_depth = (gl_FragCoord.w != 1.f) ? (inverse(projMat) * vec4(0, 0, 0, gl_FragCoord.w)).w : gl_FragCoord.z;"
                            "colors[1] = vec4(current_depth, current_alpha, current_depth, current_alpha);"
                            "colors[0] = vec4(current_depth, current_alpha, previous_depth_alpha.z, previous_depth_alpha.a);"
                            "b = (current_depth >= previous_depth_alpha.z && current_alpha != 0);"
                            "gl_FragColor = colors[int(b)];"
                        "}";
                        const std::string frameBufferGenFragShader =
                        "#version 130 \n"
                        "uniform sampler2D depthBuffer;"
                        "uniform sampler2D frameBuffer;"
                        "uniform sampler2D texture;"
                        "uniform vec3 resolution;"
                        "uniform float haveTexture;"
                        "in mat4 projMat;"
                        "void main () {"
                            "vec2 position = ( gl_FragCoord.xy / resolution.xy );"
                            "vec4 previous_depth_alpha = texture2D(depthBuffer, position);"
                            "vec4 previous_color = texture2D(frameBuffer, position);"
                            "vec4 texel = texture2D(texture, gl_TexCoord[0].xy);"
                            "vec4 colors[2];"
                            "colors[1] = texel * gl_Color;"
                            "colors[0] = gl_Color;"
                            "bool b = (haveTexture == 1);"
                            "vec4 current_color = colors[int(b)];"
                            "float current_depth = (gl_FragCoord.w != 1.f) ? (inverse(projMat) * vec4(0, 0, 0, gl_FragCoord.w)).w : gl_FragCoord.z;"
                            "colors[1] = current_color * current_color.a + previous_color * (1 - current_color.a);"
                            "colors[1].a = current_color.a + previous_color.a * (1 - current_color.a);"
                            "colors[0] = previous_color * previous_depth_alpha.a + current_color * (1 - previous_depth_alpha.a);"
                            "colors[0].a = previous_color.a + previous_depth_alpha.a * (1 - previous_color.a);"
                            "b = (current_depth >= previous_depth_alpha.z);"
                            "gl_FragColor = colors[int(b)];"
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
                                "gl_FragColor = vec4(refractionFactor, 0, 0, z);"
                            "} else {"
                                "gl_FragColor = refraction;"
                            "}"
                        "}";
                        if (!depthBufferGenerator->loadFromMemory(vertexShader, depthGenFragShader))
                            throw core::Erreur(50, "Failed to load depth buffer generator shader", 0);
                        if (!frameBufferGenerator->loadFromMemory(vertexShader, frameBufferGenFragShader))
                            throw core::Erreur(51, "Failed to load frame buffer generator shader", 0);
                        if (!specularTextureGenerator->loadFromMemory(vertexShader, specularGenFragShader))
                            throw core::Erreur(52, "Failed to load specular texture generator shader", 0);
                        if (!bumpTextureGenerator->loadFromMemory(vertexShader, bumpGenFragShader))
                            throw core::Erreur(53, "Failed to load bump texture generator shader", 0);
                        if (!refractionTextureGenerator->loadFromMemory(vertexShader, refractionGenFragShader))
                            throw core::Erreur(54, "Failed to load refraction texture generator shader", 0);
                }
                frameBufferGenerator->setParameter("resolution",resolution.x, resolution.y, resolution.z);
                frameBufferGenerator->setParameter("depthBuffer", depthBuffer->getTexture());
                frameBufferGenerator->setParameter("frameBuffer", frameBuffer->getTexture());
                frameBufferGenerator->setParameter("texture", Shader::CurrentTexture);
                depthBufferGenerator->setParameter("resolution",resolution.x, resolution.y, resolution.z);
                depthBufferGenerator->setParameter("depthBuffer", depthBuffer->getTexture());
                depthBufferGenerator->setParameter("texture", Shader::CurrentTexture);
                specularTextureGenerator->setParameter("resolution",resolution.x, resolution.y, resolution.z);
                specularTextureGenerator->setParameter("specularTexture",specularTexture->getTexture());
                specularTextureGenerator->setParameter("texture",Shader::CurrentTexture);
                specularTextureGenerator->setParameter("maxM", Material::getMaxSpecularIntensity());
                specularTextureGenerator->setParameter("maxP", Material::getMaxSpecularPower());
                bumpTextureGenerator->setParameter("resolution",resolution.x, resolution.y, resolution.z);
                bumpTextureGenerator->setParameter("bumpTexture",bumpTexture->getTexture());
                bumpTextureGenerator->setParameter("texture",Shader::CurrentTexture);
                refractionTextureGenerator->setParameter("resolution",resolution.x, resolution.y, resolution.z);
                refractionTextureGenerator->setParameter("bumpTexture",bumpTexture->getTexture());
                core::FastDelegate<bool> signal (&FastRenderComponent::needToUpdate, this);
                core::FastDelegate<void> slot (&FastRenderComponent::drawNextFrame, this);
                core::Command cmd(signal, slot);
                getListener().connect("UPDATE", cmd);
                mvpBuffer = 0;
                backgroundColor = sf::Color::Transparent;
            }
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

             if (Shader::isAvailable()) {
                batcher.clear();
                for (unsigned int i = 0; i < vEntities.size(); i++) {
                    if ( vEntities[i]->isLeaf()) {
                        for (unsigned int j = 0; j <  vEntities[i]->getFaces().size(); j++) {
                             batcher.addFace( vEntities[i]->getFaces()[j]);
                        }
                    }
                }
                std::lock_guard<std::recursive_mutex> locker(rec_mutex);
                m_instances = batcher.getInstances();
            }
            this->visibleEntities = vEntities;
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
            currentStates.blendMode = sf::BlendNone;
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
                            currentStates.transform = m_instances[i]->getTransforms()[j].get();
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
                                math::Matrix4f tmatrix = drawables[i].second.texture->getTextureMatrix().transpose();
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
                        std::lock_guard<std::recursive_mutex> locker(rec_mutex);
                        float specularIntensity = m_instances[i]->getMaterial().getSpecularIntensity();
                        float specularPower = m_instances[i]->getMaterial().getSpecularPower();
                        float refractionFactor = m_instances[i]->getMaterial().getRefractionFactor();
                        specularTextureGenerator->setParameter("m", specularIntensity);
                        specularTextureGenerator->setParameter("p", specularPower);
                        refractionTextureGenerator->setParameter("refractionFactor", refractionFactor);
                        if (m_instances[i]->getMaterial().getTexture() != nullptr) {
                            depthBufferGenerator->setParameter("haveTexture", 1.f);
                            frameBufferGenerator->setParameter("haveTexture", 1.f);
                            specularTextureGenerator->setParameter("haveTexture", 1.f);
                        } else {
                            depthBufferGenerator->setParameter("haveTexture", 0.f);
                            frameBufferGenerator->setParameter("haveTexture", 0.f);
                            specularTextureGenerator->setParameter("haveTexture", 0.f);
                        }
                        if (m_instances[i]->getMaterial().getBumpTexture() != nullptr) {
                            bumpTextureGenerator->setParameter("haveTexture", 1.f);
                        } else {
                            bumpTextureGenerator->setParameter("haveTexture", 0.f);
                        }
                        for (unsigned int j = 0; j < m_instances[i]->getVertexArrays().size(); j++) {
                            currentStates.texture = m_instances[i]->getMaterial().getTexture();
                            currentStates.transform = m_instances[i]->getTransforms()[j].get();
                            currentStates.shader = frameBufferGenerator;
                            frameBuffer->draw(*m_instances[i]->getVertexArrays()[j], currentStates);
                            currentStates.shader = depthBufferGenerator;
                            depthBuffer->draw(*m_instances[i]->getVertexArrays()[j], currentStates);
                            currentStates.shader = bumpTextureGenerator;
                            currentStates.texture = m_instances[i]->getMaterial().getBumpTexture();
                            bumpTexture->draw(*m_instances[i]->getVertexArrays()[j], currentStates);
                            currentStates.shader = refractionTextureGenerator;
                            refractionTexture->draw(*m_instances[i]->getVertexArrays()[j], currentStates);
                        }
                    }
                }
                for (unsigned int i = 0; i < drawables.size(); i++) {
                    frameBuffer->draw(drawables[i].first.get(), drawables[i].second);
                }
                depthBuffer->display();
                frameBuffer->display();
                specularTexture->display();
                bumpTexture->display();
                refractionTexture->display();
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
