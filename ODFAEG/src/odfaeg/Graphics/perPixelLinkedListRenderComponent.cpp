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
            frameBuffer.create(window.getView().getSize().x, window.getView().getSize().y, settings);
            frameBufferSprite = Sprite(frameBuffer.getTexture(), math::Vec3f(0, 0, 0), math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0), sf::IntRect(0, 0, window.getView().getSize().x, window.getView().getSize().y));
            glCheck(glGenTextures(1, &headPtrTex));
            glCheck(glBindTexture(GL_TEXTURE_2D, headPtrTex));
            glCheck(glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, window.getView().getSize().x, window.getView().getSize().y));
            glCheck(glBindImageTexture(0, headPtrTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI));
            glCheck(glBindTexture(GL_TEXTURE_2D, 0));
            glCheck(glGenBuffers(1, &atomicBuffer));
            glCheck(glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, atomicBuffer));
            glCheck(glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW));
            glCheck(glGenBuffers(1, &linkedListBuffer));
            glCheck(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, linkedListBuffer));
            glCheck(glBufferData(GL_SHADER_STORAGE_BUFFER, maxNodes * nodeSize, NULL, GL_DYNAMIC_DRAW));
            std::vector<GLuint> headPtrClearBuf(window.getView().getSize().x*window.getView().getSize().y, 0xffffffff);
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
               #extension GL_ARB_shader_subroutine : require
               #define MAX_FRAGMENTS 75
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
               subroutine vec4 RenderPassType();
               subroutine uniform RenderPassType RenderPass;
               subroutine(RenderPassType)
               vec4 pass1 () {
                   uint nodeIdx = atomicCounterIncrement(nextNodeCounter);
                   if (nodeIdx < maxNodes) {
                        uint prevHead = imageAtomicExchange(headPointers, ivec2(gl_FragCoord.xy), nodeIdx);
                        nodes[nodeIdx].color = (haveTexture > 0.9) ? gl_Color * texture2D(texture, gl_TexCoord[0].xy) : gl_Color;
                        nodes[nodeIdx].depth = gl_FragCoord.z;
                        nodes[nodeIdx].next = prevHead;
                   }
                   /*NodeType frags[MAX_FRAGMENTS];

                   int count = 0;

                   // Get the index of the head of the list
                   uint n = imageLoad(headPointers, ivec2(gl_FragCoord.xy)).r;

                   // Copy the linked list for this fragment into an array
                   while( n != uint(0xffffffff) && count < MAX_FRAGMENTS) {
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
                   vec4 color = vec4(0.5, 0.5, 0.5, 1.0);
                   for( int i = 0; i < count; i++ )
                   {
                     color = mix( color, frags[i].color, frags[i].color.a);
                   }

                   // Output the final color
                   return color;*/
                   return vec4(0, 0, 0, 0);

               }
               subroutine(RenderPassType)
               vec4 pass2 () {
                   NodeType frags[MAX_FRAGMENTS];
                   int count = 0;
                   uint n = imageLoad(headPointers, ivec2(gl_FragCoord.xy)).r;
                   while( n != uint(0xffffffff) && count < MAX_FRAGMENTS) {
                        frags[count] = nodes[n];
                        n = frags[count].next;
                        count++;
                   }
                   for (int i = 0; i < count-1; i++) {
                        for (int j = i+1; j < count; j++) {
                            if (frags[i].depth > frags[j].depth) {
                                NodeType tmp = frags[i];
                                frags[i] = frags[j];
                                frags[j] = tmp;
                            }
                        }
                   }
                   // Traverse the array, and combine the colors using the alpha
                  // channel.
                  vec4 color = vec4 (0, 0, 0, 0);
                  for (int i = 0; i < count; i++) {
                    color.rgb = frags[i].color.rgb * frags[i].color.a + color.rgb * (1 - frags[i].color.a);
                    color.a = frags[i].color.a + color.a * (1 - frags[i].color.a);
                  }
                  // Output the final color
                  return color;
               }
               void main() {
                   gl_FragColor = RenderPass();
               })";
               if (!perPixelLinkedList.loadFromMemory(simpleVertexShader, fragmentShader)) {
                    throw core::Erreur(54, "Failed to load per pixel linked list shader");
               }
               perPixelLinkedList.setParameter("maxNodes", maxNodes);
               perPixelLinkedList.setParameter("texture", Shader::CurrentTexture);
               GLuint programHandle = perPixelLinkedList.getHandle();
               pass1Index = glGetSubroutineIndex( programHandle, GL_FRAGMENT_SHADER, "pass1");
               pass2Index = glGetSubroutineIndex( programHandle, GL_FRAGMENT_SHADER, "pass2");
               backgroundColor = sf::Color::Transparent;
        }
        void PerPixelLinkedListRenderComponent::setBackgroundColor(sf::Color color) {
            backgroundColor = color;
        }
        void PerPixelLinkedListRenderComponent::clear() {
            frameBuffer.clear(backgroundColor);
            GLuint zero = 0;
            glCheck(glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, atomicBuffer));
            glCheck(glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &zero));
            glCheck(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, clearBuf));
            glCheck(glBindTexture(GL_TEXTURE_2D, headPtrTex));
            glCheck(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, view.getSize().x, view.getSize().y, GL_RED_INTEGER,
            GL_UNSIGNED_INT, NULL));
            glCheck(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0));
        }
        void PerPixelLinkedListRenderComponent::pass1() {
            Shader::bind(&perPixelLinkedList);
            glCheck(glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &pass1Index));

            currentStates.blendMode = sf::BlendAlpha;
            currentStates.shader=&perPixelLinkedList;
            for (unsigned int i = 0; i < m_instances.size(); i++) {
                if (m_instances[i].getAllVertices().getVertexCount() > 0) {
                    if (m_instances[i].getMaterial().getTexture() == nullptr) {
                        perPixelLinkedList.setParameter("haveTexture", 0.f);
                    } else {
                        perPixelLinkedList.setParameter("haveTexture", 1.f);
                    }
                    currentStates.texture=m_instances[i].getMaterial().getTexture();
                    frameBuffer.draw(m_instances[i].getAllVertices(), currentStates);
                }
            }
            frameBuffer.display();
            glCheck(glFinish());
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
                    if (ptr[i] != 4294967295) {
                            std::cout<<ptr[i];
                        if (i < size - 1)
                            std::cout<<" "<<std::endl;
                    }
                    if (i % (unsigned int) view.getSize().x == 0)
                            std::cout<<std::endl;
                }
            Texture::bind(&frameBuffer.getTexture());
                //glCheck(glBindTexture(GL_TEXTURE_2D, headPtrTex));
                size = view.getSize().x*view.getSize().y*4;
                GLubyte src[size];
                glCheck(glGetTexImage(GL_TEXTURE_2D,
                  0,
                  GL_RGBA,
                  GL_UNSIGNED_BYTE,
                  src));


                    if(src) {
                        std::cout<<"pass 1 "<<std::endl;
                        for (unsigned int i = 0; i < view.getSize().x * view.getSize().y; i++) {
                             if (src[i*4] > 0 || src[i*4+1] > 0 || src[i*4+2] > 0 || src[i*4+3] > 0) {
                                std::cout<<"color : (";
                                for (unsigned int j = 0; j < 4; j++) {

                                         std::cout<<(unsigned int) src[i*4+j];
                                         if (j < 3)
                                            std::cout<<" , ";
                                }
                                std::cout<<")"<<std::endl;
                             }
                        }

                    }
                    frameBuffer.display();
        }
        void PerPixelLinkedListRenderComponent::pass2() {
            glCheck(glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT ));
            Shader::bind(&perPixelLinkedList);
            glCheck(glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &pass2Index));
            currentStates.blendMode = sf::BlendAlpha;
            currentStates.shader=&perPixelLinkedList;
            quad.setCenter(view.getPosition());
            frameBuffer.draw(quad, currentStates);

            glCheck(glFinish());

                Texture::bind(&frameBuffer.getTexture());
                //glCheck(glBindTexture(GL_TEXTURE_2D, headPtrTex));
                unsigned int size = view.getSize().x*view.getSize().y*4;
                GLubyte src[size];
                glCheck(glGetTexImage(GL_TEXTURE_2D,
                  0,
                  GL_RGBA,
                  GL_UNSIGNED_BYTE,
                  src));


                    if(src) {
                        std::cout<<"pass 2 : "<<std::endl;
                        for (unsigned int i = 0; i < view.getSize().x * view.getSize().y; i++) {
                             if (src[i*4] > 0 || src[i*4+1] > 0 || src[i*4+2] > 0 || src[i*4+3] > 0) {
                                std::cout<<"color : (";
                                for (unsigned int j = 0; j < 4; j++) {

                                         std::cout<<(unsigned int) src[i*4+j];
                                         if (j < 3)
                                            std::cout<<" , ";
                                }
                                std::cout<<")"<<std::endl;
                             }
                        }

                    }
                    frameBuffer.display();
        }
        void PerPixelLinkedListRenderComponent::drawNextFrame() {
            pass1();
            pass2();
        }
        void PerPixelLinkedListRenderComponent::draw(RenderTarget& target, RenderStates states) {
            frameBufferSprite.setCenter(view.getPosition());
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
