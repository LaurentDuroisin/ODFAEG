#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include "../../../include/odfaeg/Graphics/perPixelLinkedListRenderComponent.hpp"
#include "glCheck.h"
#include "../../../include/odfaeg/Physics/particuleSystem.h"
namespace odfaeg {
    namespace graphic {
        PerPixelLinkedListRenderComponent::PerPixelLinkedListRenderComponent(RenderWindow& window, int layer, std::string expression, window::ContextSettings settings) :
            HeavyComponent(window, math::Vec3f(window.getView().getPosition().x, window.getView().getPosition().y, layer),
                          math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0),
                          math::Vec3f(window.getView().getSize().x + window.getView().getSize().x * 0.5f, window.getView().getPosition().y + window.getView().getSize().y * 0.5f, layer)),
            view(window.getView()),
            expression(expression),
            quad(math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, window.getSize().y * 0.5f)) {
            quad.move(math::Vec3f(-window.getView().getSize().x * 0.5f, 0/*-window.getView().getSize().y * 0.5f*/, 0));
            GLuint maxNodes = 20 * window.getView().getSize().x * window.getView().getSize().y;
            GLint nodeSize = 5 * sizeof(GLfloat) + sizeof(GLuint);
            frameBuffer.create(window.getView().getSize().x, window.getView().getSize().y, settings);
            frameBufferSprite = Sprite(frameBuffer.getTexture(), math::Vec3f(0, 0, 0), math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0), sf::IntRect(0, 0, window.getView().getSize().x, window.getView().getSize().y));
            frameBuffer.setView(view);
            //window.setActive();
            glCheck(glGenBuffers(1, &atomicBuffer));
            glCheck(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicBuffer));
            glCheck(glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW));
            glCheck(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0));
            glCheck(glGenBuffers(1, &linkedListBuffer));
            glCheck(glBindBuffer(GL_SHADER_STORAGE_BUFFER, linkedListBuffer));
            glCheck(glBufferData(GL_SHADER_STORAGE_BUFFER, maxNodes * nodeSize, NULL, GL_DYNAMIC_DRAW));
            glCheck(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
            glCheck(glGenTextures(1, &headPtrTex));
            glCheck(glBindTexture(GL_TEXTURE_2D, headPtrTex));
            glCheck(glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, window.getView().getSize().x, window.getView().getSize().y));
            glCheck(glBindImageTexture(0, headPtrTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI));
            glCheck(glBindTexture(GL_TEXTURE_2D, 0));
            std::vector<GLuint> headPtrClearBuf(window.getView().getSize().x*window.getView().getSize().y, 0xffffffff);
            std::vector<GLuint> linkedListClearBuffer(maxNodes * nodeSize, 0);
            glCheck(glGenBuffers(1, &clearBuf));
            glCheck(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, clearBuf));
            glCheck(glBufferData(GL_PIXEL_UNPACK_BUFFER, headPtrClearBuf.size() * sizeof(GLuint),
            &headPtrClearBuf[0], GL_STATIC_COPY));
            glCheck(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0));
            core::FastDelegate<bool> signal (&PerPixelLinkedListRenderComponent::needToUpdate, this);
            core::FastDelegate<void> slot (&PerPixelLinkedListRenderComponent::drawNextFrame, this);
            core::Command cmd(signal, slot);
            getListener().connect("UPDATE", cmd);

            if (settings.versionMajor >= 4 && settings.versionMinor >= 3) {
                glGenBuffers(1, &vboWorldMatrices);
                const std::string vertexShader = R"(#version 460 core
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
                                                        gl_Position = projectionMatrix * viewMatrix * worldMat * vec4(position, 1.f);
                                                        fTexCoords = (textureMatrix * vec4(texCoords, 1.f, 1.f)).xy;
                                                        frontColor = color;
                                                    }
                                                    )";
                const std::string  simpleVertexShader = R"(#version 460 core
                                                        layout (location = 0) in vec3 position;
                                                        layout (location = 1) in vec4 color;
                                                        layout (location = 2) in vec2 texCoords;
                                                        uniform mat4 projectionMatrix;
                                                        uniform mat4 viewMatrix;
                                                        uniform mat4 worldMat;
                                                        void main () {
                                                            gl_Position = projectionMatrix * viewMatrix * worldMat * vec4(position, 1.f);
                                                        })";
                const std::string fragmentShader = R"(#version 460 core
                                                      struct NodeType {
                                                          vec4 color;
                                                          float depth;
                                                          uint next;
                                                      };
                                                      layout(binding = 0, offset = 0) uniform atomic_uint nextNodeCounter;
                                                      layout(binding = 0, r32ui) uniform uimage2D headPointers;
                                                      layout(binding = 0, std430) buffer linkedLists {
                                                          NodeType nodes[];
                                                      };
                                                      uniform uint maxNodes;
                                                      uniform float haveTexture;
                                                      uniform sampler2D texture;
                                                      in vec4 frontColor;
                                                      in vec2 fTexCoords;
                                                      layout (location = 0) out vec4 fcolor;
                                                      void main() {
                                                           uint nodeIdx = atomicCounterIncrement(nextNodeCounter);
                                                           vec4 texel = texture2D(texture, fTexCoords.xy);
                                                           vec4 color = (haveTexture > 0.9) ? frontColor * texel : frontColor;
                                                           if (nodeIdx < maxNodes) {
                                                                uint prevHead = imageAtomicExchange(headPointers, ivec2(gl_FragCoord.xy), nodeIdx);
                                                                nodes[nodeIdx].color = color;
                                                                nodes[nodeIdx].depth = gl_FragCoord.z;
                                                                nodes[nodeIdx].next = prevHead;
                                                           }
                                                           fcolor = vec4(0, 0, 0, 0);
                                                      })";
                 const std::string fragmentShader2 =
                   R"(
                   #version 460
                   #define MAX_FRAGMENTS 20
                   struct NodeType {
                      vec4 color;
                      float depth;
                      uint next;
                   };
                   layout(binding = 0, r32ui) uniform uimage2D headPointers;
                   layout(binding = 0, std430) buffer linkedLists {
                       NodeType nodes[];
                   };
                   layout(location = 0) out vec4 fcolor;
                   void main() {
                      NodeType frags[MAX_FRAGMENTS];
                      int count = 0;
                      uint n = imageLoad(headPointers, ivec2(gl_FragCoord.xy)).r;
                      while( n != 0xffffffffu && count < MAX_FRAGMENTS) {
                           frags[count] = nodes[n];
                           n = frags[count].next;
                           count++;
                      }
                      //merge sort
                      int i, j1, j2, k;
                      int a, b, c;
                      int step = 1;
                      NodeType leftArray[MAX_FRAGMENTS/2]; //for merge sort

                      while (step <= count)
                      {
                          i = 0;
                          while (i < count - step)
                          {
                              ////////////////////////////////////////////////////////////////////////
                              //merge(step, i, i + step, min(i + step + step, count));
                              a = i;
                              b = i + step;
                              c = (i + step + step) >= count ? count : (i + step + step);

                              for (k = 0; k < step; k++)
                                  leftArray[k] = frags[a + k];

                              j1 = 0;
                              j2 = 0;
                              for (k = a; k < c; k++)
                              {
                                  if (b + j1 >= c || (j2 < step && leftArray[j2].depth > frags[b + j1].depth))
                                      frags[k] = leftArray[j2++];
                                  else
                                      frags[k] = frags[b + j1++];
                              }
                              ////////////////////////////////////////////////////////////////////////
                              i += 2 * step;
                          }
                          step *= 2;
                      }
                      vec4 color = vec4(0, 0, 0, 0);
                      for( int i = count - 1; i >= 0; i--)
                      {
                        color.rgb = frags[i].color.rgb * frags[i].color.a + color.rgb * (1 - frags[i].color.a);
                        color.a = frags[i].color.a + color.a * (1 - frags[i].color.a);
                      }
                      fcolor = color;
                   })";
                   if (!perPixelLinkedList.loadFromMemory(vertexShader, fragmentShader)) {
                        throw core::Erreur(54, "Failed to load per pixel linked list shader");
                   }
                   std::cout<<"ppll shaders 1 compilated"<<std::endl;
                   if (!perPixelLinkedListP2.loadFromMemory(simpleVertexShader, fragmentShader2)) {
                        throw core::Erreur(55, "Failed to load per pixel linked list pass 2 shader");
                   }
                   /*if (!filterNotOpaque.loadFromMemory(simpleVertexShader, filterNotOpaquePixels)) {
                        throw core::Erreur(54, "Failed to load filter not opaque shader");
                   }*/
                   perPixelLinkedList.setParameter("maxNodes", maxNodes);
                   perPixelLinkedList.setParameter("texture", Shader::CurrentTexture);
            } else {
                const std::string  simpleVertexShader =
                R"(#version 140
                void main () {
                    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
                    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
                    gl_FrontColor = gl_Color;
                })";
                const std::string fragmentShader =
                R"(#version 140
                   #extension GL_ARB_shader_atomic_counters : require
                   #extension GL_ARB_shading_language_420pack : require
                   #extension GL_ARB_shader_image_load_store : require
                   #extension GL_ARB_shader_storage_buffer_object : require
                   struct NodeType {
                      vec4 color;
                      float depth;
                      uint next;
                   };
                   layout(binding = 0, offset = 0) uniform atomic_uint nextNodeCounter;
                   layout(binding = 0, r32ui) uniform uimage2D headPointers;
                   layout(binding = 0, std430) buffer linkedLists {
                       NodeType nodes[];
                   };
                   uniform uint maxNodes;
                   uniform float haveTexture;
                   uniform sampler2D texture;
                   void main() {
                       uint nodeIdx = atomicCounterIncrement(nextNodeCounter);
                       vec4 texel = texture2D(texture, gl_TexCoord[0].xy);
                       vec4 color = (haveTexture > 0.9) ? gl_Color * texel : gl_Color;
                       if (nodeIdx < maxNodes) {
                            uint prevHead = imageAtomicExchange(headPointers, ivec2(gl_FragCoord.xy), nodeIdx);
                            nodes[nodeIdx].color = color;
                            nodes[nodeIdx].depth = gl_FragCoord.z;
                            nodes[nodeIdx].next = prevHead;
                       }
                       gl_FragColor = vec4(0, 0, 0, 0);
                   })";
                   const std::string fragmentShader2 =
                   R"(
                   #version 140
                   #extension GL_ARB_shader_atomic_counters : require
                   #extension GL_ARB_shading_language_420pack : require
                   #extension GL_ARB_shader_image_load_store : require
                   #extension GL_ARB_shader_storage_buffer_object : require
                   #define MAX_FRAGMENTS 20
                   struct NodeType {
                      vec4 color;
                      float depth;
                      uint next;
                   };
                   layout(binding = 0, r32ui) uniform uimage2D headPointers;
                   layout(binding = 0, std430) buffer linkedLists {
                       NodeType nodes[];
                   };
                   void main() {
                      NodeType frags[MAX_FRAGMENTS];
                      int count = 0;
                      uint n = imageLoad(headPointers, ivec2(gl_FragCoord.xy)).r;
                      while( n != 0xffffffffu && count < MAX_FRAGMENTS) {
                           frags[count] = nodes[n];
                           n = frags[count].next;
                           count++;
                      }
                      //merge sort
                      int i, j1, j2, k;
                      int a, b, c;
                      int step = 1;
                      NodeType leftArray[MAX_FRAGMENTS/2]; //for merge sort

                      while (step <= count)
                      {
                          i = 0;
                          while (i < count - step)
                          {
                              ////////////////////////////////////////////////////////////////////////
                              //merge(step, i, i + step, min(i + step + step, count));
                              a = i;
                              b = i + step;
                              c = (i + step + step) >= count ? count : (i + step + step);

                              for (k = 0; k < step; k++)
                                  leftArray[k] = frags[a + k];

                              j1 = 0;
                              j2 = 0;
                              for (k = a; k < c; k++)
                              {
                                  if (b + j1 >= c || (j2 < step && leftArray[j2].depth > frags[b + j1].depth))
                                      frags[k] = leftArray[j2++];
                                  else
                                      frags[k] = frags[b + j1++];
                              }
                              ////////////////////////////////////////////////////////////////////////
                              i += 2 * step;
                          }
                          step *= 2;
                      }
                      vec4 color = vec4(0, 0, 0, 0);
                      for( int i = count - 1; i >= 0; i--)
                      {
                        color.rgb = frags[i].color.rgb * frags[i].color.a + color.rgb * (1 - frags[i].color.a);
                        color.a = frags[i].color.a + color.a * (1 - frags[i].color.a);
                      }
                      gl_FragColor = color;
                   })";
                   /*if (!initialize.loadFromMemory(simpleVertexShader, initializeSSBO)) {
                        throw core::Erreur(54, "Failed to load initialize ssbo shader");
                   } */
                   if (!perPixelLinkedList.loadFromMemory(simpleVertexShader, fragmentShader)) {
                        throw core::Erreur(54, "Failed to load per pixel linked list shader");
                   }
                   if (!perPixelLinkedListP2.loadFromMemory(simpleVertexShader, fragmentShader2)) {
                        throw core::Erreur(54, "Failed to load per pixel linked list pass 2 shader");
                   }
                   std::cout<<"shaders compilated"<<std::endl;
                   /*if (!filterNotOpaque.loadFromMemory(simpleVertexShader, filterNotOpaquePixels)) {
                        throw core::Erreur(54, "Failed to load filter not opaque shader");
                   }*/
                   perPixelLinkedList.setParameter("maxNodes", maxNodes);
                   perPixelLinkedList.setParameter("texture", Shader::CurrentTexture);
                }
               //filterNotOpaque.setParameter("texture", Shader::CurrentTexture);
               backgroundColor = sf::Color::Transparent;
               glCheck(glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, atomicBuffer));
               glCheck(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, linkedListBuffer));
        }
        void PerPixelLinkedListRenderComponent::setBackgroundColor(sf::Color color) {
            backgroundColor = color;
        }
        void PerPixelLinkedListRenderComponent::clear() {
            frameBuffer.setActive();
            frameBuffer.clear(backgroundColor);
            //getWindow().setActive();
            GLuint zero = 0;
            glCheck(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicBuffer));
            glCheck(glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &zero));
            glCheck(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0));
            glCheck(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, clearBuf));
            glCheck(glBindTexture(GL_TEXTURE_2D, headPtrTex));
            glCheck(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, view.getSize().x, view.getSize().y, GL_RED_INTEGER,
            GL_UNSIGNED_INT, NULL));
            glCheck(glBindTexture(GL_TEXTURE_2D, 0));
            frameBuffer.resetGLStates();
            //getWindow().resetGLStates();*/

        }
        void PerPixelLinkedListRenderComponent::drawNextFrame() {
            if (frameBuffer.getSettings().versionMajor >= 4 && frameBuffer.getSettings().versionMinor >= 3) {
                math::Matrix4f viewMatrix = view.getViewMatrix().getMatrix().transpose();
                math::Matrix4f projMatrix = view.getProjMatrix().getMatrix().transpose();
                perPixelLinkedList.setParameter("projectionMatrix", projMatrix);
                perPixelLinkedList.setParameter("viewMatrix", viewMatrix);
                for (unsigned int i = 0; i < m_instances.size(); i++) {
                    if (m_instances[i].getAllVertices().getVertexCount() > 0) {
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
                        currentStates.blendMode = sf::BlendNone;
                        currentStates.shader = &perPixelLinkedList;
                        currentStates.texture = m_instances[i].getMaterial().getTexture();
                        if (m_instances[i].getMaterial().getTexture() != nullptr) {
                            math::Matrix4f texMatrix = m_instances[i].getMaterial().getTexture()->getTextureMatrix();
                            perPixelLinkedList.setParameter("textureMatrix", texMatrix);
                            perPixelLinkedList.setParameter("haveTexture", 1.f);
                        } else {
                            perPixelLinkedList.setParameter("haveTexture", 0.f);
                        }
                        frameBuffer.drawInstanced(vb, vboWorldMatrices, m_instances[i].getVertexArrays()[0]->getPrimitiveType(), 0, m_instances[i].getVertexArrays()[0]->getVertexCount(), tm.size(), currentStates);
                    }
                }
                glCheck(glFinish());
                glCheck(glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT));
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
                vb2.clear();
                vb2.setPrimitiveType(sf::Quads);
                Vertex v1 (sf::Vector3f(0, 0, 0));
                Vertex v2 (sf::Vector3f(quad.getSize().x,0, 0));
                Vertex v3 (sf::Vector3f(quad.getSize().x, quad.getSize().y, quad.getSize().z));
                Vertex v4 (sf::Vector3f(0, quad.getSize().y, quad.getSize().z));
                vb2.append(v1);
                vb2.append(v2);
                vb2.append(v3);
                vb2.append(v4);
                vb2.update();
                math::Matrix4f matrix = quad.getTransform().getMatrix().transpose();
                perPixelLinkedListP2.setParameter("projectionMatrix", projMatrix);
                perPixelLinkedListP2.setParameter("viewMatrix", viewMatrix);
                perPixelLinkedListP2.setParameter("worldMat", matrix);
                currentStates.shader = &perPixelLinkedListP2;
                frameBuffer.drawVertexBuffer(vb2, currentStates);
                glCheck(glFinish());
                frameBuffer.display();
            } else {
                currentStates.blendMode = sf::BlendNone;
                currentStates.shader=&perPixelLinkedList;

                for (unsigned int i = 0; i < m_instances.size(); i++) {
                   if (m_instances[i].getAllVertices().getVertexCount() > 0) {
                        if (m_instances[i].getMaterial().getTexture() == nullptr) {
                            perPixelLinkedList.setParameter("haveTexture", 0.f);
                        } else {
                            perPixelLinkedList.setParameter("haveTexture", 1.f);
                        }
                        currentStates.texture = m_instances[i].getMaterial().getTexture();
                        frameBuffer.draw(m_instances[i].getAllVertices(), currentStates);
                    }
                }
                glCheck(glFinish());
                glCheck(glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT));

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


                currentStates.shader = &perPixelLinkedListP2;
                /*for (unsigned int i = 0; i < m_instances.size(); i++) {
                   if (m_instances[i].getAllVertices().getVertexCount() > 0) {
                        frameBuffer.draw(m_instances[i].getAllVertices(), currentStates);
                   }

                //glCheck(glDepthMask(GL_TRUE));*/

                //quad.setCenter(frameBuffer.getView().getPosition());
                frameBuffer.draw(quad, currentStates);
                glCheck(glFinish());
                frameBuffer.display();
            }
        }
        void PerPixelLinkedListRenderComponent::draw(RenderTarget& target, RenderStates states) {
            /*states.shader=&perPixelLinkedList;
            for (unsigned int i = 0; i < m_instances.size(); i++) {
               if (m_instances[i].getAllVertices().getVertexCount() > 0) {
                    if (m_instances[i].getMaterial().getTexture() == nullptr) {
                        perPixelLinkedList.setParameter("haveTexture", 0.f);
                        filterNotOpaque.setParameter("haveTexture", 0.f);
                    } else {
                        perPixelLinkedList.setParameter("haveTexture", 1.f);
                        filterNotOpaque.setParameter("haveTexture", 1.f);
                    }
                    states.texture = m_instances[i].getMaterial().getTexture();
                    target.draw(m_instances[i].getAllVertices(), states);

                }
            }
            glCheck(glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT));
            glCheck(glFinish());

            //glCheck(glTextureBarrier());

            states.shader = &perPixelLinkedListP2;

            //glCheck(glDepthMask(GL_FALSE));
            /*for (unsigned int i = 0; i < m_instances.size(); i++) {
               if (m_instances[i].getAllVertices().getVertexCount() > 0) {
                    target.draw(m_instances[i].getAllVertices(), states);
               }
            }*/

            /*target.draw(quad, states);
            glCheck(glFinish());*/
            frameBufferSprite.setCenter(target.getView().getPosition());
            target.draw(frameBufferSprite, states);

        }
        int  PerPixelLinkedListRenderComponent::getLayer() {
            return getPosition().z;
        }
        void PerPixelLinkedListRenderComponent::draw(Drawable& drawable, RenderStates states) {
            //drawables.insert(std::make_pair(drawable, states));
        }
        void PerPixelLinkedListRenderComponent::changeVisibleEntities(Entity* toRemove, Entity* toAdd, EntityManager* em) {
        }
        void PerPixelLinkedListRenderComponent::setView(View view) {
            math::Vec3f t = view.getPosition() - this->view.getPosition();
            quad.move(t);
            frameBuffer.setView(view);
            this->view = view;
        }
        std::vector<Entity*> PerPixelLinkedListRenderComponent::getEntities() {
            return visibleEntities;
        }
        std::string PerPixelLinkedListRenderComponent::getExpression() {
            return expression;
        }
        View& PerPixelLinkedListRenderComponent::getView() {
            return view;
        }
        bool PerPixelLinkedListRenderComponent::needToUpdate() {
            return update;
        }
        void PerPixelLinkedListRenderComponent::setExpression (std::string expression) {
            this->expression = expression;
        }
        bool PerPixelLinkedListRenderComponent::loadEntitiesOnComponent(std::vector<Entity*> vEntities) {
            batcher.clear();
            for (unsigned int i = 0; i < vEntities.size(); i++) {
                //if ( vEntities[i]->isLeaf()) {
                    for (unsigned int j = 0; j <  vEntities[i]->getNbFaces(); j++) {
                         batcher.addFace( vEntities[i]->getFace(j));
                    }
                //}
            }
            m_instances = batcher.getInstances();
            visibleEntities = vEntities;
            update = true;
            return true;
        }
        void PerPixelLinkedListRenderComponent::updateParticleSystems() {
            for (unsigned int i = 0; i < visibleEntities.size(); i++) {
                if (dynamic_cast<physic::ParticleSystem*>(visibleEntities[i]) != nullptr) {
                    static_cast<physic::ParticleSystem*>(visibleEntities[i])->update();
                }
            }
            loadEntitiesOnComponent(visibleEntities);
            update = true;
        }
        void PerPixelLinkedListRenderComponent::pushEvent(window::IEvent event, RenderWindow& rw) {
            if (event.type == window::IEvent::WINDOW_EVENT && event.window.type == window::IEvent::WINDOW_EVENT_RESIZED && &getWindow() == &rw && isAutoResized()) {
                std::cout<<"recompute size"<<std::endl;
                recomputeSize();
                getListener().pushEvent(event);
                getView().reset(physic::BoundingBox(getView().getViewport().getPosition().x, getView().getViewport().getPosition().y, getView().getViewport().getPosition().z, event.window.data1, event.window.data2, getView().getViewport().getDepth()));
            }
        }
        PerPixelLinkedListRenderComponent::~PerPixelLinkedListRenderComponent() {
            glDeleteBuffers(1, &atomicBuffer);
            glDeleteBuffers(1, &linkedListBuffer);
            glDeleteBuffers(1, &clearBuf);
            glDeleteBuffers(1, &clearBuf2);
            glDeleteTextures(1, &headPtrTex);
        }
    }
}
