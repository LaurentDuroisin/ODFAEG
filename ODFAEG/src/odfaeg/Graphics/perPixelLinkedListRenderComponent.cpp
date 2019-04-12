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
            quad(math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0)) {
            GLuint maxNodes = 20 * window.getView().getSize().x * window.getView().getSize().y;
            GLint nodeSize = 5 * sizeof(GLfloat) + sizeof(GLuint);
            /*frameBuffer.create(window.getView().getSize().x, window.getView().getSize().y, settings);
            frameBufferSprite = Sprite(frameBuffer.getTexture(), math::Vec3f(0, 0, 0), math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0), sf::IntRect(0, 0, window.getView().getSize().x, window.getView().getSize().y));*/
            window.setActive();
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
            std::vector<GLuint> headPtrClearBuf(window.getView().getSize().x*window.getView().getSize().y, 0);
            glCheck(glGenBuffers(1, &clearBuf));
            glCheck(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, clearBuf));
            glCheck(glBufferData(GL_PIXEL_UNPACK_BUFFER, headPtrClearBuf.size() * sizeof(GLuint),
            &headPtrClearBuf[0], GL_STATIC_COPY));
            glCheck(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0));
            core::FastDelegate<bool> signal (&PerPixelLinkedListRenderComponent::needToUpdate, this);
            core::FastDelegate<void> slot (&PerPixelLinkedListRenderComponent::drawNextFrame, this);
            core::Command cmd(signal, slot);
            getListener().connect("UPDATE", cmd);
            const std::string  simpleVertexShader =
            R"(#version 140
            void main () {
                gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
                gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
                gl_FrontColor = gl_Color;
            })";
            const std::string filterNotOpaquePixels =
            R"(#version 140
            uniform sampler2D texture;
            uniform float haveTexture;
            void main () {
                vec4 texel = texture2D(texture, gl_TexCoord[0].xy);
                vec4 colors[2];
                colors[1] = texel * gl_Color;
                colors[0] = gl_Color;
                bool b = (haveTexture > 0.9);
                vec4 color = colors[int(b)];
                colors[0] = vec4(0, 0, 0, 0);
                colors[1] = color;
                b = (color.a == 1);
                gl_FragColor = colors[int(b)];
            })";
            const std::string fragmentShader =
            R"(#version 140
               #extension GL_ARB_shader_atomic_counters : require
               #extension GL_ARB_shading_language_420pack : require
               #extension GL_ARB_shader_image_load_store : require
               #extension GL_ARB_shader_storage_buffer_object : require
               #define MAX_FRAGMENTS 75
               struct NodeType {
                  vec4 color;
                  float depth;
                  uint next;
               };
               layout(binding = 0, offset = 0) uniform atomic_uint nextNodeCounter;
               layout(binding = 0, r32ui) coherent uniform uimage2D headPointers;
               layout(binding = 0, std430) coherent buffer linkedLists {
                   NodeType nodes[];
               };
               uniform uint maxNodes;
               uniform float haveTexture;
               uniform sampler2D texture;
               in mat4 projMat;
               void main() {
                   uint nodeIdx = atomicCounterIncrement(nextNodeCounter) + 1u;
                   vec4 texel = texture2D(texture, gl_TexCoord[0].xy);
                   vec4 color = (haveTexture > 0.9) ? gl_Color * texel : gl_Color;
                   if (nodeIdx < maxNodes && color.a != 0 && color.a != 1) {
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
               #define MAX_FRAGMENTS 75
               struct NodeType {
                  vec4 color;
                  float depth;
                  uint next;
               };
               layout(binding = 0, r32ui) coherent uniform uimage2D headPointers;
               layout(binding = 0, std430) coherent buffer linkedLists {
                   NodeType nodes[];
               };
               void main() {
                  NodeType frags[MAX_FRAGMENTS];
                  NodeType frags2[MAX_FRAGMENTS];
                  float depth[MAX_FRAGMENTS];
                  int count = 0;
                  uint n = imageLoad(headPointers, ivec2(gl_FragCoord.xy)).r;
                  while( n != 0u && count < MAX_FRAGMENTS) {
                       frags[count] = nodes[n];
                       frags2[count] = frags[count];
                       depth[count] = frags[count].depth;
                       n = nodes[n].next;
                       count++;
                  }
                  // Sort the array by depth using insertion sort (largest
                  // to smallest).

                  //insert sort

                  /*for( int i = 1; i < count; i++ )
                  {
                    NodeType toInsert = frags[i];
                    int j = i;
                    while( j > 0 && toInsert.depth > frags[j-1].depth ) {
                      frags[j] = frags[j-1];
                      j--;
                    }
                    frags[j] = toInsert;
                  }*/

                  //selection sort

                  /*int max;
                  NodeType tempNode;
                  int j, i;
                  for(j = 0; j < count - 1; j++)
                  {
                    max = j;
                    for(  i = j + 1; i < count; i++)
                    {
                        if(frags[i].depth > frags[max].depth)
                        {
                            max = i;
                        }
                    }
                    if(max != j)
                    {
                        tempNode = frags[j];
                        frags[j] = frags[max];
                        frags[max] = tempNode;
                    }
                  }*/



                  //bubble sort
                  /*int j, i;
                  NodeType tempNode;
                  float tempDepth;
                  for(i = 0; i < count - 1; i++)
                  {
                    for(j = 0; j < count - i - 1; j++)
                    {
                        if(depth[j] < depth[j+1])
                        {
                            tempNode = frags[j];
                            frags[j] = frags[j+1];
                            frags[j+1] = tempNode;
                            tempDepth = depth[j];
                            depth[j] = depth[j+1];
                            depth[j+1] = tempDepth;
                        }
                    }
                  }*/
                 /* int i, j;
                    NodeType tmp;
                    int inc = count / 2;
                    while (inc > 0)
                    {
                        for (i = inc; i < count; i++)
                        {
                            tmp = frags[i];
                            j = i;
                            while (j >= inc && frags[j - inc].depth < tmp.depth)
                            {
                                frags[j] = frags[j - inc];
                                j -= inc;
                            }
                            frags[j] = tmp;
                        }
                        inc = int(inc / 2.2 + 0.5);
                    }*/



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
                  //Tri indexé.
                  /*int inds[MAX_FRAGMENTS];
                  for (int i = 0; i < count; i++) {
                    inds[i] = i;
                  }
                  int j, i;
                  int temp;
                  for(i = 0; i < count - 1; i++)
                  {
                    for(j = 0; j < count - i - 1; j++)
                    {
                        if(frags2[j].depth < frags2[j+1].depth)
                        {
                            temp = inds[j];
                            inds[j] = inds[j+1];
                            inds[j+1] = temp;
                        }
                    }
                  }*/
                  vec4 color = vec4(0, 0, 0, 0);
                  float mdepth = 0.f;
                  for( int i = 0; i < count; i++ )
                  {
                      color = mix( color, frags[i].color, frags[i].color.a);
                      //color = vec4(frags[inds[i]].color.rgb * frags[inds[i]].color.a + color.rgb * (1 - frags[inds[i]].color.a), frags[inds[i]].color.a + color.a * (1 - frags[inds[i]].color.a));
                      if (frags2[i].color.r > 1 || frags2[i].color.g > 1 || frags2[i].color.b > 1 || frags2[i].color.a > 1)
                        color = color;
                  }
                  gl_FragColor = color;
               })";
               if (!perPixelLinkedList.loadFromMemory(simpleVertexShader, fragmentShader)) {
                    throw core::Erreur(54, "Failed to load per pixel linked list shader");
               }
               if (!perPixelLinkedListP2.loadFromMemory(simpleVertexShader, fragmentShader2)) {
                    throw core::Erreur(54, "Failed to load per pixel linked list pass 2 shader");
               }
               if (!filterNotOpaque.loadFromMemory(simpleVertexShader, filterNotOpaquePixels)) {
                    throw core::Erreur(54, "Failed to load filter not opaque shader");
               }
               perPixelLinkedList.setParameter("maxNodes", maxNodes);
               perPixelLinkedList.setParameter("texture", Shader::CurrentTexture);
               filterNotOpaque.setParameter("texture", Shader::CurrentTexture);
               backgroundColor = sf::Color::Transparent;
        }
        void PerPixelLinkedListRenderComponent::setBackgroundColor(sf::Color color) {
            backgroundColor = color;
        }
        void PerPixelLinkedListRenderComponent::clear() {
            /*frameBuffer.setActive();
            frameBuffer.clear(backgroundColor);*/
            getWindow().setActive();
            GLuint zero = 0;
            glCheck(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicBuffer));
            glCheck(glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &zero));
            glCheck(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0));
            glCheck(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, clearBuf));
            glCheck(glBindTexture(GL_TEXTURE_2D, headPtrTex));
            glCheck(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, view.getSize().x, view.getSize().y, GL_RED_INTEGER,
            GL_UNSIGNED_INT, NULL));
            glCheck(glBindTexture(GL_TEXTURE_2D, 0));
            /*GLuint maxNodes = 20 * view.getSize().x * view.getSize().y;
            GLint nodeSize = 5 * sizeof(GLfloat) + sizeof(GLuint);
            glCheck(glBindBuffer(GL_SHADER_STORAGE_BUFFER, linkedListBuffer));
            std::vector<GLfloat> data(maxNodes * nodeSize, 0);
            glCheck(glBufferData(GL_SHADER_STORAGE_BUFFER, maxNodes * nodeSize, &data[0], GL_DYNAMIC_DRAW));
            glCheck(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));*/
            //frameBuffer.resetGLStates();
            getWindow().resetGLStates();
            glCheck(glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, atomicBuffer));
            glCheck(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, linkedListBuffer));
        }
        void PerPixelLinkedListRenderComponent::drawNextFrame() {
            /*frameBuffer.setActive();
            currentStates.blendMode = sf::BlendAlpha;
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
            frameBuffer.display();
            glCheck(glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, 0));
            glCheck(glTextureBarrier());
            glCheck(glMemoryBarrier( GL_TEXTURE_FETCH_BARRIER_BIT ));
            glCheck(glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT ));
            currentStates.shader = &perPixelLinkedListP2;
            for (unsigned int i = 0; i < m_instances.size(); i++) {
               if (m_instances[i].getAllVertices().getVertexCount() > 0) {
                    frameBuffer.draw(m_instances[i].getAllVertices(), currentStates);
               }
            }
            glCheck(glFinish());
            frameBuffer.display();*/

        }
        void PerPixelLinkedListRenderComponent::draw(RenderTarget& target, RenderStates states) {
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
                    states.shader=&perPixelLinkedList;
                    target.draw(m_instances[i].getAllVertices(), states);
                    states.shader=&filterNotOpaque;
                    target.draw(m_instances[i].getAllVertices(), states);
                }
            }
            glCheck(glFinish());
            glCheck(glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, 0));
            //glCheck(glTextureBarrier());
            //glCheck(glMemoryBarrier( GL_TEXTURE_FETCH_BARRIER_BIT ));
            glCheck(glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT ));
            states.shader = &perPixelLinkedListP2;
            states.blendMode = sf::BlendAlpha;
            glCheck(glDepthMask(GL_FALSE));
            for (unsigned int i = 0; i < m_instances.size(); i++) {
               if (m_instances[i].getAllVertices().getVertexCount() > 0) {
                    target.draw(m_instances[i].getAllVertices(), states);
               }
            }
            glCheck(glDepthMask(GL_TRUE));
            //target.draw(quad, states);
            glCheck(glFinish());
            /*frameBufferSprite.setCenter(target.getView().getPosition());
            target.draw(frameBufferSprite, states);*/
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
            glDeleteTextures(1, &headPtrTex);
        }
    }
}
