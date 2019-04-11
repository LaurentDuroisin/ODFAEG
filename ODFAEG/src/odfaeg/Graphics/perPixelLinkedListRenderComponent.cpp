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
               void main() {
                   vec4 texel = texture2D(texture, gl_TexCoord[0].xy);
                   vec4 color = (haveTexture > 0.9) ? gl_Color * texel : gl_Color;
                   if (color.a > 0) {
                       uint nodeIdx = atomicCounterIncrement(nextNodeCounter) + 1u;
                       if (nodeIdx < maxNodes) {
                            uint prevHead = imageAtomicExchange(headPointers, ivec2(gl_FragCoord.xy), nodeIdx);
                            nodes[nodeIdx].color = color;
                            nodes[nodeIdx].depth = gl_FragCoord.z;
                            nodes[nodeIdx].next = prevHead;
                       }
                   }
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
                  int count = 0;
                  uint n = imageLoad(headPointers, ivec2(gl_FragCoord.xy)).r;
                  while( n != 0u && count < MAX_FRAGMENTS) {
                       frags[count] = nodes[n];
                       n = frags[count].next;
                       imageStore(headPointers, ivec2(gl_FragCoord.xy), uvec4(n, 0, 0, 0));
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
                  for( int i = 0; i < count; i++ )
                  {
                      color = mix( color, frags[i].color, frags[i].color.a);
                  }
                  gl_FragColor = color;
               })";
               if (!perPixelLinkedList.loadFromMemory(simpleVertexShader, fragmentShader)) {
                    throw core::Erreur(54, "Failed to load per pixel linked list shader");
               }
               if (!perPixelLinkedListP2.loadFromMemory(simpleVertexShader, fragmentShader2)) {
                    throw core::Erreur(54, "Failed to load per pixel linked list pass 2 shader");
               }
               perPixelLinkedList.setParameter("maxNodes", maxNodes);
               perPixelLinkedList.setParameter("texture", Shader::CurrentTexture);
               backgroundColor = sf::Color::Transparent;
        }
        void PerPixelLinkedListRenderComponent::setBackgroundColor(sf::Color color) {
            backgroundColor = color;
        }
        void PerPixelLinkedListRenderComponent::clear() {
            GLuint zero = 0;
            glCheck(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicBuffer));
            glCheck(glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &zero));
            glCheck(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0));
            glCheck(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, clearBuf));
            glCheck(glBindTexture(GL_TEXTURE_2D, headPtrTex));
            glCheck(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, view.getSize().x, view.getSize().y, GL_RED_INTEGER,
            GL_UNSIGNED_INT, NULL));
            glCheck(glBindTexture(GL_TEXTURE_2D, 0));
            getWindow().resetGLStates();
            glCheck(glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, atomicBuffer));
            glCheck(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, linkedListBuffer));
        }
        void PerPixelLinkedListRenderComponent::drawNextFrame() {
            //Nothing to do because I draw on the window directly, not on a FBO.
        }
        void PerPixelLinkedListRenderComponent::draw(RenderTarget& target, RenderStates states) {
            states.blendMode = sf::BlendAlpha;
            states.shader=&perPixelLinkedList;

            for (unsigned int i = 0; i < m_instances.size(); i++) {
               if (m_instances[i].getAllVertices().getVertexCount() > 0) {
                    if (m_instances[i].getMaterial().getTexture() == nullptr) {
                        perPixelLinkedList.setParameter("haveTexture", 0.f);
                    } else {
                        perPixelLinkedList.setParameter("haveTexture", 1.f);
                    }
                    states.texture = m_instances[i].getMaterial().getTexture();
                    target.draw(m_instances[i].getAllVertices(), states);
                }
            }
            glCheck(glFinish());
            glCheck(glTextureBarrier());
            glCheck(glMemoryBarrier( GL_TEXTURE_FETCH_BARRIER_BIT ));
            glCheck(glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT ));
            glCheck(glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, 0));
            states.shader = &perPixelLinkedListP2;
            target.draw(quad, states);
            glCheck(glFinish());
            glCheck(glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT ));
            glCheck(glBindTexture(GL_TEXTURE_2D, headPtrTex));
                unsigned int size = view.getSize().x*view.getSize().y;
                GLuint ptr[size];
                glCheck(glGetTexImage(GL_TEXTURE_2D,
                  0,
                  GL_RED_INTEGER,
                  GL_UNSIGNED_INT,
                 ptr));
                std::cout<<"ptrs : "<<std::endl;
                for (unsigned int i = 0; i < size; i++) {
                    if (ptr[i] != 0) {
                            std::cout<<ptr[i];
                        if (i < size - 1)
                            std::cout<<" "<<std::endl;
                        if (i % (unsigned int) view.getSize().x == 0)
                            std::cout<<std::endl;
                    }

                }
            glCheck(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0));
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
