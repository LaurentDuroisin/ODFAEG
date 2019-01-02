#include "../../../include/odfaeg/Graphics/oitRenderComponent.h"
#include "glCheck.h"
#include <memory.h>
using namespace sf;
using namespace std;
namespace odfaeg {
    namespace graphic {
        OITRenderComponent::OITRenderComponent (RenderWindow& window, int layer, std::string expression, sf::ContextSettings settings) :
            HeavyComponent(window, math::Vec3f(window.getView().getPosition().x, window.getView().getPosition().y, layer),
                          math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0),
                          math::Vec3f(window.getView().getSize().x + window.getView().getSize().x * 0.5f, window.getView().getPosition().y + window.getView().getSize().y * 0.5f, layer)),
            view(window.getView()),
            expression(expression) {
            update = false;
            sf::Vector3i resolution ((int) window.getSize().x, (int) window.getSize().y, window.getView().getSize().z);
            depthBuffer = std::make_unique<RenderTexture>();
            frameBuffer = std::make_unique<RenderTexture>();
            specularTexture = std::make_unique<RenderTexture>();
            bumpTexture = std::make_unique<RenderTexture>();
            refractionTexture = std::make_unique<RenderTexture>();
            depthBuffer->create(resolution.x, resolution.y,settings);
            frameBuffer->create(resolution.x, resolution.y,settings);
            specularTexture->create(resolution.x, resolution.y,settings);
            bumpTexture->create(resolution.x, resolution.y,settings);
            refractionTexture->create(resolution.x, resolution.y,settings);
            frameBuffer->setView(window.getView());
            depthBuffer->setView(window.getView());
            specularTexture->setView(window.getView());
            bumpTexture->setView(window.getView());
            refractionTexture->setView(window.getView());
            frameBufferTile = std::make_unique<Tile>(&frameBuffer->getTexture(), math::Vec3f(0, 0, 0), math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0), IntRect(0, 0, window.getView().getSize().x, window.getView().getSize().y));
            depthBufferTile = std::make_unique<Tile>(&depthBuffer->getTexture(), math::Vec3f(0, 0, 0), math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0), IntRect(0, 0, window.getView().getSize().x, window.getView().getSize().y));
            core::FastDelegate<bool> signal (&OITRenderComponent::needToUpdate, this);
            core::FastDelegate<void> slot (&OITRenderComponent::drawNextFrame, this);
            core::Command cmd(signal, slot);
            getListener().connect("UPDATE", cmd);
            if (Shader::isAvailable()) {
                frameBufferGenerator = std::make_unique<Shader>();
                depthBufferGenerator = std::make_unique<Shader>();
                specularTextureGenerator = std::make_unique<Shader>();
                bumpTextureGenerator = std::make_unique<Shader>();
                refractionTextureGenerator = std::make_unique<Shader>();
                simpleShader = std::make_unique<Shader>();
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
                    "colors[1] = vec4(0, 0, current_depth, current_alpha);"
                    "colors[0] = vec4(0, 0, previous_depth_alpha.z, previous_depth_alpha.a);"
                    "b = (current_depth >= previous_depth_alpha.z);"
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
                    "colors[0] = previous_color * previous_color.a + current_color * (1 - previous_color.a);"
                    "colors[0].a = previous_color.a + current_color.a * (1 - previous_color.a);"
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
                        "gl_FragColor = vec4(refractionFactor, 0, z, 0);"
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
                frameBufferGenerator->setParameter("resolution",resolution.x, resolution.y, resolution.z);
                frameBufferGenerator->setParameter("depthBuffer", depthBuffer->getTexture());
                frameBufferGenerator->setParameter("frameBuffer", frameBuffer->getTexture());
                frameBufferGenerator->setParameter("texture", Shader::CurrentTexture);
                depthBufferGenerator->setParameter("texture", Shader::CurrentTexture);
                depthBufferGenerator->setParameter("resolution",resolution.x, resolution.y, resolution.z);
                depthBufferGenerator->setParameter("depthBuffer",depthBuffer->getTexture());
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
                backgroundColor = sf::Color::Transparent;
            } else {
                throw core::Erreur(55, "Shader not supported!", 0);
            }
        }
        void OITRenderComponent::pushEvent(sf::Event event, RenderWindow& rw) {
            if (event.type == sf::Event::Resized && &getWindow() == &rw && isAutoResized()) {
                std::cout<<"recompute size"<<std::endl;
                recomputeSize();
                getListener().pushEvent(event);
                getView().reset(physic::BoundingBox(getView().getViewport().getPosition().x, getView().getViewport().getPosition().y, getView().getViewport().getPosition().z, event.size.width, event.size.height, getView().getViewport().getDepth()));
            }
        }
        bool OITRenderComponent::needToUpdate() {
            return update;
        }
        void OITRenderComponent::changeVisibleEntities(Entity* toRemove, Entity* toAdd, EntityManager* em) {
            bool removed;
            em->removeAnimatedVisibleEntity(toRemove, visibleEntities, view, removed);
            if (removed) {
                em->insertAnimatedVisibleEntity(toAdd, visibleEntities, view);
                loadEntitiesOnComponent(visibleEntities);
                update = true;
            }
        }
        std::string OITRenderComponent::getExpression() {
            return expression;
        }
        void OITRenderComponent::setBackgroundColor(sf::Color color) {
            this->backgroundColor = color;
        }
        void OITRenderComponent::clear() {
             frameBuffer->clear(backgroundColor);
             depthBuffer->clear(sf::Color::Transparent);
             specularTexture->clear(sf::Color::Transparent);
             bumpTexture->clear(sf::Color::Transparent);
             refractionTexture->clear(sf::Color::Transparent);
        }
        Tile& OITRenderComponent::getFrameBufferTile () {
            return *frameBufferTile;
        }
        Tile& OITRenderComponent::getDepthBufferTile() {
            return *depthBufferTile;
        }
        const Texture& OITRenderComponent::getDepthBufferTexture() {
            return depthBuffer->getTexture();
        }
        const Texture& OITRenderComponent::getFrameBufferTexture() {
            return frameBuffer->getTexture();
        }
        const Texture& OITRenderComponent::getSpecularTexture() {
            return specularTexture->getTexture();
        }
        bool OITRenderComponent::loadEntitiesOnComponent(std::vector<Entity*> vEntities)
        {

            batcher.clear();
            for (unsigned int i = 0; i < vEntities.size(); i++) {
                if ( vEntities[i]->isLeaf()) {
                    for (unsigned int j = 0; j <  vEntities[i]->getFaces().size(); j++) {
                         batcher.addFace( vEntities[i]->getFaces()[j]);
                    }
                }
            }
            m_instances = batcher.getInstances();
            visibleEntities = vEntities;
            update = true;
            return true;
        }
        void OITRenderComponent::setView(View view) {
            this->view = view;
            frameBuffer->setView(view);
            depthBuffer->setView(view);
            specularTexture->setView(view);
            bumpTexture->setView(view);
            refractionTexture->setView(view);
        }
        void OITRenderComponent::setExpression(std::string expression) {
            update = true;
            this->expression = expression;
        }
        void OITRenderComponent::drawNextFrame() {
            update = false;
            currentStates.blendMode = sf::BlendNone;
            for (unsigned int i = 0; i < m_instances.size(); i++) {
                /*float specularIntensity = m_instances[i].getMaterial().getSpecularIntensity();
                float specularPower = m_instances[i].getMaterial().getSpecularPower();
                float refractionFactor = m_instances[i].getMaterial().getRefractionFactor();
                specularTextureGenerator->setParameter("m", specularIntensity);
                specularTextureGenerator->setParameter("p", specularPower);
                refractionTextureGenerator->setParameter("refractionFactor", refractionFactor);*/
                if (m_instances[i].getMaterial().getTexture() != nullptr) {
                    depthBufferGenerator->setParameter("haveTexture", 1.f);
                    frameBufferGenerator->setParameter("haveTexture", 1.f);
                    //specularTextureGenerator->setParameter("haveTexture", 1.f);
                } else {
                    depthBufferGenerator->setParameter("haveTexture", 0.f);
                    frameBufferGenerator->setParameter("haveTexture", 0.f);
                    //specularTextureGenerator->setParameter("haveTexture", 0.f);
                }
                /*if (m_instances[i].getMaterial().getBumpTexture() != nullptr) {
                    bumpTextureGenerator->setParameter("haveTexture", 1.f);
                } else {
                    bumpTextureGenerator->setParameter("haveTexture", 0.f);
                }*/
                for (unsigned int j = 0; j < m_instances[i].getVertexArrays().size(); j++) {
                    currentStates.texture = m_instances[i].getMaterial().getTexture();
                    currentStates.transform = m_instances[i].getTransforms()[j];
                    currentStates.shader = frameBufferGenerator.get();
                    frameBuffer->draw(m_instances[i].getVertexArrays()[j], currentStates);
                    currentStates.shader = depthBufferGenerator.get();
                    depthBuffer->draw(m_instances[i].getVertexArrays()[j], currentStates);
                    /*currentStates.shader = refractionTextureGenerator.get();
                    refractionTexture->draw(m_instances[i].getVertexArrays()[j], currentStates);
                    currentStates.shader = bumpTextureGenerator.get();
                    currentStates.texture = m_instances[i].getMaterial().getBumpTexture();
                    bumpTexture->draw(m_instances[i].getVertexArrays()[j], currentStates);*/
                }
            }
            for (unsigned int i = 0; i < drawables.size(); i++) {
                frameBuffer->draw(drawables[i].first.get(), drawables[i].second);
            }
            depthBuffer->display();
            frameBuffer->display();
            frameBufferTile->setPosition(view.getPosition());
            depthBufferTile->setPosition(view.getPosition());
            specularTexture->display();
            bumpTexture->display();
            refractionTexture->display();
        }
        std::vector<Entity*> OITRenderComponent::getEntities() {
            return visibleEntities;
        }
        void OITRenderComponent::draw(Drawable& drawable, RenderStates states) {
            update = true;
            drawables.push_back(std::make_pair(std::ref(drawable), states));
        }
        void OITRenderComponent::draw(RenderTarget& target, RenderStates states) {
            frameBufferTile->setCenter(target.getView().getPosition());
            target.draw(*frameBufferTile, states);
            drawables.clear();
        }

        View& OITRenderComponent::getView() {
            return view;
        }
        int OITRenderComponent::getLayer() {
            return getPosition().z;
        }
        const Texture& OITRenderComponent::getBumpTexture() {
            return bumpTexture->getTexture();
        }
        const Texture& OITRenderComponent::getRefractionTexture() {
            return refractionTexture->getTexture();
        }
        void OITRenderComponent::updateParticleSystems() {
            for (unsigned int i = 0; i < visibleEntities.size(); i++) {
                if (dynamic_cast<physic::ParticleSystem*>(visibleEntities[i]) != nullptr) {
                    static_cast<physic::ParticleSystem*>(visibleEntities[i])->update();
                }
            }
            loadEntitiesOnComponent(visibleEntities);
            update = true;
        }
    }
}
