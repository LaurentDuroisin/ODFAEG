#include "../../../include/odfaeg/Graphics/fastRenderComponent.h"
#include "glCheck.h"
#include <memory.h>
using namespace sf;
using namespace std;
namespace odfaeg {
    namespace graphic {
        FastRenderComponent::FastRenderComponent (RenderWindow& window, int layer, std::string expression, bool useThread) :
            Component(math::Vec3f(window.getView().getPosition().x, window.getView().getPosition().y, layer),
                          math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0),
                          math::Vec3f(window.getView().getSize().x + window.getView().getSize().x * 0.5f, window.getView().getPosition().y + window.getView().getSize().y * 0.5f, layer), useThread),
            view(window.getView()),
            expression(expression) {
            update = false;
            sf::Vector3i resolution ((int) window.getSize().x, (int) window.getSize().y, window.getView().getSize().z);
            depthBuffer = new RenderTexture();
            frameBuffer = new RenderTexture();
            depthBuffer->create(resolution.x, resolution.y,window.getSettings());
            frameBuffer->create(resolution.x, resolution.y,window.getSettings());
            frameBuffer->setView(window.getView());
            depthBuffer->setView(window.getView());
            frameBuffer->clear(sf::Color::Transparent);
            depthBuffer->clear(sf::Color::Transparent);
            frameBufferTile = new Tile(&frameBuffer->getTexture(), math::Vec3f(0, 0, 0), math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0), IntRect(0, 0, window.getView().getSize().x, window.getView().getSize().y));
            depthBufferTile = new Tile(&depthBuffer->getTexture(), math::Vec3f(0, 0, 0), math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0), IntRect(0, 0, window.getView().getSize().x, window.getView().getSize().y));
            if (Shader::isAvailable()) {

                frameBufferGenerator = new Shader();
                depthBufferGenerator = new Shader();
                //With modern openg, we need to use another vertex shader.
                //GLSL 3 is not supported by all the drivers so we use GLSL 1.
                if (Shader::getShadingLanguageVersionMajor() >= 3 && Shader::getShadingLanguageVersionMinor() >= 3) {
                    const std::string vertexShader =
                    "#version 330 core \n"
                    "layout(location = 0) in vec3 vertex_position;"
                    "layout(location = 1) in vec4 vertex_color;"
                    "layout(location = 2) in vec2 vertex_texCoords0;"
                    "layout(location = 3) in vec3 vertex_normal;"
                    "/*layout(location = 10) in mat4 mvp;*/"
                    "uniform mat4 m;"
                    "uniform mat4 v;"
                    "uniform mat4 p;"
                    "out vec2 texCoords;"
                    "out vec4 color;"
                    "out mat4 projMat;"
                    "void main () {"
                        "gl_Position = m * v * p * vertex_position.xyzw;"
                        "texCoords = vertex_texCoords0;"
                        "projMat = p;"
                        "color = vertex_color;"
                    "}";
                    const std::string depthGenFragShader =
                    "#version 330 core \n"
                    "layout(origin_upper_left, pixel_center_integer) in vec4 gl_FragCoord;"
                    "uniform sampler2D depthBuffer;"
                    "uniform sampler2D texture;"
                    "uniform vec3 resolution;"
                    "in vec2 texCoords;"
                    "in vec4 color;"
                    "out vec4 fragColor;"
                    "in mat4 projMat;"
                    "void main () {"
                          "vec2 position = ( gl_FragCoord.xy / resolution.xy );"
                          "vec4 depth = texture2D(depthBuffer, position);"
                          "vec4 pixel = texture2D(texture, texCoords.xy);"
                          "float z = (gl_FragCoord.w != 1.f) ? (inverse(projMat) * vec4(0, 0, 0, gl_FragCoord.w)).w : gl_FragCoord.z;"
                          "/*if (z <= color.z && pixel.a >= color.a) {*/"
                              "fragColor = vec4(0, 0,z, pixel.a);"
                          "/*} else {"
                              "fragColor = depth;"
                          "}*/"
                    "}";
                    const std::string frameBufferGenFragShader =
                    "#version 330 core \n"
                    "layout(origin_upper_left, pixel_center_integer) in vec4 gl_FragCoord;"
                    "uniform sampler2D depthBuffer;"
                    "uniform sampler2D frameBuffer;"
                    "uniform sampler2D texture;"
                    "uniform vec3 resolution;"
                    "in vec2 texCoords;"
                    "in vec4 color;"
                    "out vec4 fragColor;"
                    "in mat4 projMat;"
                    "void main () {"
                        "vec2 position = ( gl_FragCoord.xy / resolution.xy );"
                        "vec4 depth = texture2D(depthBuffer, position);"
                        "vec4 fbColor = texture2D(frameBuffer, position);"
                        "vec4 pixel = texture2D(texture, texCoords.xy);"
                        "float z = (gl_FragCoord.w != 1.f) ? (inverse(projMat) * vec4(0, 0, 0, gl_FragCoord.w)).w : gl_FragCoord.z;"
                        "if (z <= depth.z) {"
                            "fragColor = pixel;"
                        "} else {"
                            "fragColor = fbColor;"
                        "}"
                    "}";
                const std::string normalMapGenFragShader =
                    "#version 330 core \n"
                    "layout(origin_upper_left, pixel_center_integer) in vec4 gl_FragCoord;"
                    "const vec2 size = vec2(2.0,0.0);"
                    "const ivec3 off = ivec3(-1,0,1);"
                    "uniform sampler2D texture;"
                    "in vec2 texCoords;"
                    "in vec4 color;"
                    "out vec4 fragColor;"
                    "void main() {"
                    "vec4 depth = texture2D(texture, texCoords[0].xy);"
                    "if (pixel.r != 0 || pixel.g != 0 || pixel.b != 0 || pixel.a != 0) {"
                        "float s01 = textureOffset(texture, texCoords.xy, off.xy).z;"
                        "float s21 = textureOffset(texture, texCoords.xy, off.zy).z;"
                        "float s10 = textureOffset(texture, texCoords.xy, off.yx).z;"
                        "float s12 = textureOffset(texture, texCoords.xy, off.yz).z;"
                        "vec3 va = normalize (vec3(size.xy, s21 - s01));"
                        "vec3 vb = normalize (vec3(size.yx, s12 - s10));"
                        "fragColor = vec4(cross(va, vb), depth.z);"
                    "} else {"
                        "fragColor = vec4(0, 0, 0, 0);"
                    "}"
                    "}";
                    if (!depthBufferGenerator->loadFromMemory(vertexShader, depthGenFragShader))
                        throw core::Erreur(50, "Failed to load depth buffer generator shader", 0);
                    if (!frameBufferGenerator->loadFromMemory(vertexShader, frameBufferGenFragShader))
                        throw core::Erreur(51, "Failed to load frame buffer generator shader", 0);
                } else {
                   std::cout<<"version 130"<<std::endl;
                   const std::string  vertexShader =
                   "#version 130 \n"
                   "out mat4 projMat;"
                   "void main () {"
                        "gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
                        "gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;"
                        "gl_FrontColor = gl_Color;"
                        "projMat = gl_ProjectionMatrix;"
                   "}";
                    const std::string depthGenFragShader =
                        "#version 130 \n"
                        "uniform sampler2D depthBuffer;"
                        "uniform sampler2D heightMap;"
                        "uniform sampler2D texture;"
                        "uniform vec3 resolution;"
                        "uniform float haveTexture;"
                        "uniform float maxM;"
                        "uniform float maxP;"
                        "/*uniform float m;"
                        "uniform float p;*/"
                        "in mat4 projMat;"
                        "void main () {"
                            "vec2 position = ( gl_FragCoord.xy / resolution.xy );"
                            "vec4 color = texture2D(depthBuffer, position);"
                            "vec4 pixel = (haveTexture==1) ? gl_Color * texture2D(texture, gl_TexCoord[0].xy) : gl_Color;"
                            "float z = (gl_FragCoord.w != 1.f) ? (inverse(projMat) * vec4(0, 0, 0, gl_FragCoord.w)).w : gl_FragCoord.z;"
                            "/*float intensity = (maxM != 0) ? m / maxM : 0.f;"
                            "float power = (maxP != 0) ? p / maxP : 0.f;*/"
                            "if (z >= color.z && pixel.a >= color.a) {"
                                "gl_FragColor = vec4(z, pixel.a, z, pixel.a);"
                            "} else {"
                                "gl_FragColor = vec4(z, pixel.a, color.b, color.a);"
                            "}"
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
                            "vec4 depth = texture2D(depthBuffer, position);"
                            "vec4 color = texture2D(frameBuffer, position);"
                            "vec4 pixel = (haveTexture==1) ? gl_Color * texture2D(texture, gl_TexCoord[0].xy) : gl_Color;"
                            "float z = (gl_FragCoord.w != 1.f) ? (inverse(projMat) * vec4(0, 0, 0, gl_FragCoord.w)).w : gl_FragCoord.z;"
                            "if (z >= depth.z) {"
                                "/*gl_FragColor = pixel * pixel.a + color * (1 - pixel.a);"
                                "gl_FragColor.a = pixel.a + depth.g * (1 - pixel.a);*/"
                                "gl_FragColor = pixel;"
                            "} else {"
                                "/*gl_FragColor = color * depth.g + pixel * (1 - depth.g);"
                                "gl_FragColor.a = color.a + pixel.a * (1 - color.a);*/"
                                "gl_FragColor = color;"
                            "}"
                        "}";
                        if (!depthBufferGenerator->loadFromMemory(vertexShader, depthGenFragShader))
                            throw core::Erreur(50, "Failed to load depth buffer generator shader", 0);
                        if (!frameBufferGenerator->loadFromMemory(vertexShader, frameBufferGenFragShader))
                            throw core::Erreur(51, "Failed to load frame buffer generator shader", 0);
                }
                frameBufferGenerator->setParameter("resolution",resolution.x, resolution.y, resolution.z);
                frameBufferGenerator->setParameter("depthBuffer", depthBuffer->getTexture());
                frameBufferGenerator->setParameter("frameBuffer", frameBuffer->getTexture());
                frameBufferGenerator->setParameter("texture", Shader::CurrentTexture);
                depthBufferGenerator->setParameter("resolution",resolution.x, resolution.y, resolution.z);
                depthBufferGenerator->setParameter("depthBuffer", depthBuffer->getTexture());
                depthBufferGenerator->setParameter("texture", Shader::CurrentTexture);
                depthBufferGenerator->setParameter("maxM", Material::getMaxSpecularIntensity());
                depthBufferGenerator->setParameter("maxP", Material::getMaxSpecularPower());
                /*if (window.getSettings().majorVersion >= 3) {
                    frameBufferGenerator->bindAttribute(0, "vertex_position");
                    depthBufferGenerator->bindAttribute(0, "vertex_position");
                    frameBufferGenerator->bindAttribute(1, "vertex_color");
                    depthBufferGenerator->bindAttribute(1, "vertex_color");
                    frameBufferGenerator->bindAttribute(2, "vertex_texCoord0");
                    depthBufferGenerator->bindAttribute(2, "vertex_texCoord0");

                }*/
                mvpBuffer = 0;
                backgroundColor = sf::Color::Transparent;
            }
        }
        void FastRenderComponent::changeVisibleEntities(Entity* toRemove, Entity* toAdd, EntityManager* em) {
            bool removed;
            em->removeAnimatedVisibleEntity(toRemove, visibleEntities, view, removed);
            if (removed) {
                em->insertAnimatedVisibleEntity(toAdd, visibleEntities, view);
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
                m_instances = batcher.getInstances();
            }
            this->visibleEntities = vEntities;
            return true;
        }
        void FastRenderComponent::setView(View view) {
            this->view = view;
            frameBuffer->setView(view);
            depthBuffer->setView(view);
        }
        void FastRenderComponent::setExpression(std::string expression) {
            this->expression = expression;
        }
        void FastRenderComponent::drawNextFrame() {

            if (update) {
                loadEntitiesOnComponent(visibleEntities);
                update = false;
            }
            //currentStates.blendMode = sf::BlendNone;
            if (Shader::isAvailable()) {
                if (Shader::getShadingLanguageVersionMajor() >= 3 && Shader::getShadingLanguageVersionMinor() >= 3) {
                    std::vector<float> instancesMVP;
                    math::Matrix4f vmatrix = view.getViewMatrix().getMatrix().transpose();
                    math::Matrix4f pmatrix = view.getProjMatrix().getMatrix().transpose();
                    frameBufferGenerator->setParameter("v", vmatrix);
                    frameBufferGenerator->setParameter("p", pmatrix);
                    depthBufferGenerator->setParameter("v", vmatrix);
                    depthBufferGenerator->setParameter("p", pmatrix);

                    for (unsigned int i = 0; i < m_instances.size(); i++) {
                        instancesMVP.clear();
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
                    }
                    /*for (unsigned int i = 0; i < m_instances.size(); i++) {
                        currentStates.texture = m_instances[i]->getMaterial().getTexture();
                        for (unsigned int j = 0; j < m_instances[i]->getVertexArrays().size(); j++) {
                            math::Matrix4f mmatrix = m_instances[i]->getTransforms()[j].get().getMatrix().transpose();
                            math::Matrix4f vmatrix = view.getViewMatrix().getMatrix().transpose();
                            math::Matrix4f pmatrix = view.getProjMatrix().getMatrix().transpose();
                            frameBufferGenerator->setParameter("m", mmatrix);
                            frameBufferGenerator->setParameter("v", vmatrix);
                            frameBufferGenerator->setParameter("p", pmatrix);
                            currentStates.shader = frameBufferGenerator;
                            frameBuffer->draw(*m_instances[i]->getVertexArrays()[j], currentStates);
                            frameBuffer->display();
                            depthBufferGenerator->setParameter("m", mmatrix);
                            depthBufferGenerator->setParameter("v", vmatrix);
                            depthBufferGenerator->setParameter("p", mmatrix);
                            currentStates.shader = depthBufferGenerator;
                            depthBuffer->draw(*m_instances[i]->getVertexArrays()[j], currentStates);
                            depthBuffer->display();
                        }
                    }*/
                } else {
                    for (unsigned int i = 0; i < m_instances.size(); i++) {
                        currentStates.texture = m_instances[i]->getMaterial().getTexture();
                        float specularIntensity = m_instances[i]->getMaterial().getSpecularIntensity();
                        float specularPower = m_instances[i]->getMaterial().getSpecularPower();
                        /*depthBufferGenerator->setParameter("m", specularIntensity);
                        depthBufferGenerator->setParameter("p", specularPower);*/
                        if (currentStates.texture != nullptr) {

                            depthBufferGenerator->setParameter("haveTexture", 1.f);
                            frameBufferGenerator->setParameter("haveTexture", 1.f);
                        } else {

                            depthBufferGenerator->setParameter("haveTexture", 0.f);
                            frameBufferGenerator->setParameter("haveTexture", 0.f);
                        }
                        VertexArray& va = m_instances[i]->getVertexArray();
                        currentStates.shader = frameBufferGenerator;
                        frameBuffer->draw(va, currentStates);
                        currentStates.shader = depthBufferGenerator;
                        depthBuffer->draw(va, currentStates);
                        /*for (unsigned int j = 0; j < va.getVertexCount(); j++) {
                            std::cout<<va[j].position.x<<" "<<va[j].position.y<<" "<<va[j].position.z<<std::endl;
                        }*/
                        /*for (unsigned int j = 0; j < m_instances[i]->getVertexArrays().size(); j++) {
                            //currentStates.transform = m_instances[i]->getTransforms()[j].get();
                            currentStates.shader = frameBufferGenerator;
                            frameBuffer->draw(*m_instances[i]->getVertexArrays()[j], currentStates);
                            currentStates.shader = depthBufferGenerator;
                            depthBuffer->draw(*m_instances[i]->getVertexArrays()[j], currentStates);
                        }*/
                    }
                }
                depthBuffer->display();
                frameBuffer->display();
            } else {
                for (unsigned int i = 0; i < visibleEntities.size(); i++) {
                    for (unsigned int j = 0; j < visibleEntities[i]->getFaces().size(); j++) {
                        currentStates.texture = visibleEntities[i]->getFaces()[j]->getMaterial().getTexture();
                        currentStates.transform = visibleEntities[i]->getTransform();
                        frameBuffer->draw(visibleEntities[i]->getFaces()[j]->getVertexArray(), currentStates);
                    }
                }
            }
        }
        std::vector<Entity*> FastRenderComponent::getEntities() {
            return visibleEntities;
        }
        void FastRenderComponent::draw(Drawable& drawable, RenderStates states) {
            frameBuffer->draw(drawable, states);
            frameBuffer->display();
        }
        void FastRenderComponent::draw(RenderTarget& target, RenderStates states) {
            states.transform = getTransform();
            target.draw(*frameBufferTile, states);
        }

        View& FastRenderComponent::getView() {
            return view;
        }
        int FastRenderComponent::getLayer() {
            return getPosition().z;
        }
        FastRenderComponent::~FastRenderComponent() {
            delete frameBuffer;
            delete frameBufferGenerator;
            delete depthBuffer;
            delete depthBufferGenerator;
            delete frameBufferTile;
            delete depthBufferTile;

            GLuint mvp = reinterpret_cast<GLuint>(mvpBuffer);
            glCheck(glDeleteBuffers(1, &mvp));
        }
    }
}
