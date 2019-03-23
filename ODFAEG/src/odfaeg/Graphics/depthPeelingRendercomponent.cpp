#include "../../../include/odfaeg/Graphics/depthPeelingRenderComponent.hpp"
#include "glCheck.h"
#include <memory.h>
using namespace sf;
using namespace std;
/*Works but too slow*/
namespace odfaeg {
    namespace graphic {
        DepthPeelingRenderComponent::DepthPeelingRenderComponent (RenderWindow& window, int layer, std::string expression, window::ContextSettings settings) :
            HeavyComponent(window, math::Vec3f(window.getView().getPosition().x, window.getView().getPosition().y, layer),
                          math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0),
                          math::Vec3f(window.getView().getSize().x + window.getView().getSize().x * 0.5f, window.getView().getPosition().y + window.getView().getSize().y * 0.5f, layer)),
            view(window.getView()),
            expression(expression) {
            std::cout<<"settings : "<<settings.versionMajor<<"."<<settings.versionMinor<<std::endl;
            update = false;
            sf::Vector3i resolution ((int) window.getSize().x, (int) window.getSize().y, window.getView().getSize().z);
            settings.depthBits = 0;
            frameBuffer.create(resolution.x, resolution.y,settings);
            previousFrame.create(resolution.x, resolution.y, settings);
            nbLayersCount.create(resolution.x, resolution.y, settings);
            glCheck(glGenBuffers(1, &atomicsBuffer));
            glCheck(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicsBuffer));
            glCheck(glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW));
            glCheck(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0));
            glCheck(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicsBuffer));
            glCheck(glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, atomicsBuffer));

            copiedDepthBuffer.create(resolution.x, resolution.y, settings);
            settings.depthBits = 24;
            frontBuffer.create(resolution.x, resolution.y,settings);
            depthBuffer.create(resolution.x, resolution.y,settings);
            //frameBuffer.setAlphaTestEnabled(false);
            frontBuffer.setView(window.getView());
            depthBuffer.setView(window.getView());
            frameBuffer.setView(window.getView());
            previousFrame.setView(window.getView());
            copiedDepthBuffer.setView(window.getView());
            nbLayersCount.setView(window.getView());
            //averageBlendEqPart2.setAlphaTestEnabled(false);
            layersCountSprite = Sprite (nbLayersCount.getTexture(), math::Vec3f(0, 0, 0), math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0), IntRect(0, 0, window.getView().getSize().x, window.getView().getSize().y));
            copiedDepthBufferSprite = Sprite (copiedDepthBuffer.getTexture(), math::Vec3f(0, 0, 0), math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0), IntRect(0, 0, window.getView().getSize().x, window.getView().getSize().y));
            frontBufferSprite = Sprite (frontBuffer.getTexture(), math::Vec3f(0, 0, 0), math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0), IntRect(0, 0, window.getView().getSize().x, window.getView().getSize().y));
            depthBufferSprite = Sprite (depthBuffer.getTexture(), math::Vec3f(0, 0, 0), math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0), IntRect(0, 0, window.getView().getSize().x, window.getView().getSize().y));
            frameBufferSprite = Sprite(frameBuffer.getTexture(), math::Vec3f(0, 0, 0), math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0), IntRect(0, 0, window.getView().getSize().x, window.getView().getSize().y));
            previousFrameBufferSprite = Sprite(previousFrame.getTexture(), math::Vec3f(0, 0, 0), math::Vec3f(view.getSize().x, view.getSize().y, 0), sf::IntRect(0, 0, view.getSize().x, view.getSize().y));
            core::FastDelegate<bool> signal (&DepthPeelingRenderComponent::needToUpdate, this);
            core::FastDelegate<void> slot (&DepthPeelingRenderComponent::drawNextFrame, this);
            core::Command cmd(signal, slot);
            getListener().connect("UPDATE", cmd);
            viewUpdated = true;
            firstDraw = true;
            secondDraw = false;
            animationUpdated = false;
            previousView = view;
            nbLayers = 0;
            glCheck(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicsBuffer));
            GLuint* ptr = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint),
                                                        GL_MAP_WRITE_BIT |
                                                        GL_MAP_INVALIDATE_BUFFER_BIT |
                                                        GL_MAP_UNSYNCHRONIZED_BIT);
                memset(ptr, 0, sizeof(GLuint));
                glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
                glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
            // bind the buffer and define its initial storage capacity
            if (Shader::isAvailable()) {
                const std::string  vertexShader =
                R"(#version 130
                out mat4 projMat;
                void main () {
                    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
                    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
                    gl_FrontColor = gl_Color;
                    projMat = gl_ProjectionMatrix;
                })";
                const std::string  simpleVertexShader =
                R"(#version 130
                void main () {
                    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
                    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
                    gl_FrontColor = gl_Color;
                })";
                const std::string transfFragVertexShader =
                R"(#version 130
                out mat4 projMat;
                out mat4 newViewMatrix;
                void main() {
                    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
                    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
                    gl_FrontColor = gl_Color;
                    projMat = gl_ProjectionMatrix;
                    newViewMatrix = gl_ModelViewMatrix;
                })";
                const std::string transfFragFragmentShader =
                R"(#version 130
                uniform mat4 oldViewMatrix;
                uniform mat4 viewportMatrix;
                in mat4 projMat;
                in mat4 newViewMatrix;
                uniform vec3 resolution;
                uniform sampler2D oldFrameBuffer;
                uniform vec2 uoffsets;
                mat4 inverse(mat4 m) {
                         float
                         a00 = m[0][0], a01 = m[0][1], a02 = m[0][2], a03 = m[0][3],
                         a10 = m[1][0], a11 = m[1][1], a12 = m[1][2], a13 = m[1][3],
                         a20 = m[2][0], a21 = m[2][1], a22 = m[2][2], a23 = m[2][3],
                         a30 = m[3][0], a31 = m[3][1], a32 = m[3][2], a33 = m[3][3],
                         b00 = a00 * a11 - a01 * a10,
                         b01 = a00 * a12 - a02 * a10,
                         b02 = a00 * a13 - a03 * a10,
                         b03 = a01 * a12 - a02 * a11,
                         b04 = a01 * a13 - a03 * a11,
                         b05 = a02 * a13 - a03 * a12,
                         b06 = a20 * a31 - a21 * a30,
                         b07 = a20 * a32 - a22 * a30,
                         b08 = a20 * a33 - a23 * a30,
                         b09 = a21 * a32 - a22 * a31,
                         b10 = a21 * a33 - a23 * a31,
                         b11 = a22 * a33 - a23 * a32,
                         det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;
                         return mat4(
                                     a11 * b11 - a12 * b10 + a13 * b09,
                                     a02 * b10 - a01 * b11 - a03 * b09,
                                     a31 * b05 - a32 * b04 + a33 * b03,
                                     a22 * b04 - a21 * b05 - a23 * b03,
                                     a12 * b08 - a10 * b11 - a13 * b07,
                                     a00 * b11 - a02 * b08 + a03 * b07,
                                     a32 * b02 - a30 * b05 - a33 * b01,
                                     a20 * b05 - a22 * b02 + a23 * b01,
                                     a10 * b10 - a11 * b08 + a13 * b06,
                                     a01 * b08 - a00 * b10 - a03 * b06,
                                     a30 * b04 - a31 * b02 + a33 * b00,
                                     a21 * b02 - a20 * b04 - a23 * b00,
                                     a11 * b07 - a10 * b09 - a12 * b06,
                                     a00 * b09 - a01 * b07 + a02 * b06,
                                     a31 * b01 - a30 * b03 - a32 * b00,
                                     a20 * b03 - a21 * b01 + a22 * b00) / det;
                }
                void main() {
                    vec4 ndc_coords = inverse(viewportMatrix) * gl_FragCoord;
                    vec4 clip_coords = inverse(projMat) * ndc_coords;
                    clip_coords /= clip_coords.w;
                    vec4 eyes_coords = inverse(newViewMatrix) * clip_coords;
                    eyes_coords = oldViewMatrix * eyes_coords;
                    clip_coords = projMat * eyes_coords;
                    clip_coords /= clip_coords.w;
                    vec4 old_FragCoord = viewportMatrix * clip_coords;
                    /*Doesn't work : it didn't get me the pixel of the previous frame at the previous fragment position.*/
                    vec2 position = (vec2(gl_FragCoord.x + uoffsets.x, gl_FragCoord.y - uoffsets.y) / resolution.xy);
                    vec4 colors[2];
                    colors[0] = vec4(0, 0, 0, 0);
                    colors[1] = texture2D(oldFrameBuffer, position);
                    bool b = (position.x >= 0 && position.y >= 0 && position.x <= resolution.x && position.y <= resolution.y);
                    gl_FragColor = colors[int(b)];
                })";
                const std::string simpleFragShader =
                R"(#version 130
                uniform float haveTexture;
                uniform sampler2D texture;
                void main() {
                    vec4 texel = texture2D(texture, gl_TexCoord[0].xy);
                    vec4 colors[2];
                    colors[0] = gl_Color;
                    colors[1] = texel * gl_Color;
                    bool b = (haveTexture > 0.9);
                    gl_FragColor = colors[int(b)];
                })";
                const std::string depthGenFragShader =
                R"(#version 130
                in mat4 projMat;
                uniform sampler2D texture;
                uniform float haveTexture;
                mat4 inverse(mat4 m) {
                         float
                         a00 = m[0][0], a01 = m[0][1], a02 = m[0][2], a03 = m[0][3],
                         a10 = m[1][0], a11 = m[1][1], a12 = m[1][2], a13 = m[1][3],
                         a20 = m[2][0], a21 = m[2][1], a22 = m[2][2], a23 = m[2][3],
                         a30 = m[3][0], a31 = m[3][1], a32 = m[3][2], a33 = m[3][3],
                         b00 = a00 * a11 - a01 * a10,
                         b01 = a00 * a12 - a02 * a10,
                         b02 = a00 * a13 - a03 * a10,
                         b03 = a01 * a12 - a02 * a11,
                         b04 = a01 * a13 - a03 * a11,
                         b05 = a02 * a13 - a03 * a12,
                         b06 = a20 * a31 - a21 * a30,
                         b07 = a20 * a32 - a22 * a30,
                         b08 = a20 * a33 - a23 * a30,
                         b09 = a21 * a32 - a22 * a31,
                         b10 = a21 * a33 - a23 * a31,
                         b11 = a22 * a33 - a23 * a32,
                         det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;
                         return mat4(
                                     a11 * b11 - a12 * b10 + a13 * b09,
                                     a02 * b10 - a01 * b11 - a03 * b09,
                                     a31 * b05 - a32 * b04 + a33 * b03,
                                     a22 * b04 - a21 * b05 - a23 * b03,
                                     a12 * b08 - a10 * b11 - a13 * b07,
                                     a00 * b11 - a02 * b08 + a03 * b07,
                                     a32 * b02 - a30 * b05 - a33 * b01,
                                     a20 * b05 - a22 * b02 + a23 * b01,
                                     a10 * b10 - a11 * b08 + a13 * b06,
                                     a01 * b08 - a00 * b10 - a03 * b06,
                                     a30 * b04 - a31 * b02 + a33 * b00,
                                     a21 * b02 - a20 * b04 - a23 * b00,
                                     a11 * b07 - a10 * b09 - a12 * b06,
                                     a00 * b09 - a01 * b07 + a02 * b06,
                                     a31 * b01 - a30 * b03 - a32 * b00,
                                     a20 * b03 - a21 * b01 + a22 * b00) / det;
                }
                void main () {
                    vec4 texel = texture2D(texture, gl_TexCoord[0].xy);
                    vec4 colors[2];
                    colors[1] = texel * gl_Color;
                    colors[0] = gl_Color;
                    bool b = (haveTexture > 0.9);
                    vec4 color = colors[int(b)];
                    float z = (gl_FragCoord.w != 1.f) ? (inverse(projMat) * vec4(0, 0, 0, gl_FragCoord.w)).w : gl_FragCoord.z;
                    gl_FragColor = vec4(0, 0, z, color.a);
                })";
                const std::string frameBufferGenFragShader =
                R"(#version 130
                in mat4 projMat;
                uniform sampler2D texture;
                uniform float haveTexture;
                uniform vec3 resolution;
                uniform sampler2D depthBuffer;
                mat4 inverse(mat4 m) {
                         float
                         a00 = m[0][0], a01 = m[0][1], a02 = m[0][2], a03 = m[0][3],
                         a10 = m[1][0], a11 = m[1][1], a12 = m[1][2], a13 = m[1][3],
                         a20 = m[2][0], a21 = m[2][1], a22 = m[2][2], a23 = m[2][3],
                         a30 = m[3][0], a31 = m[3][1], a32 = m[3][2], a33 = m[3][3],
                         b00 = a00 * a11 - a01 * a10,
                         b01 = a00 * a12 - a02 * a10,
                         b02 = a00 * a13 - a03 * a10,
                         b03 = a01 * a12 - a02 * a11,
                         b04 = a01 * a13 - a03 * a11,
                         b05 = a02 * a13 - a03 * a12,
                         b06 = a20 * a31 - a21 * a30,
                         b07 = a20 * a32 - a22 * a30,
                         b08 = a20 * a33 - a23 * a30,
                         b09 = a21 * a32 - a22 * a31,
                         b10 = a21 * a33 - a23 * a31,
                         b11 = a22 * a33 - a23 * a32,
                         det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;
                         return mat4(
                                     a11 * b11 - a12 * b10 + a13 * b09,
                                     a02 * b10 - a01 * b11 - a03 * b09,
                                     a31 * b05 - a32 * b04 + a33 * b03,
                                     a22 * b04 - a21 * b05 - a23 * b03,
                                     a12 * b08 - a10 * b11 - a13 * b07,
                                     a00 * b11 - a02 * b08 + a03 * b07,
                                     a32 * b02 - a30 * b05 - a33 * b01,
                                     a20 * b05 - a22 * b02 + a23 * b01,
                                     a10 * b10 - a11 * b08 + a13 * b06,
                                     a01 * b08 - a00 * b10 - a03 * b06,
                                     a30 * b04 - a31 * b02 + a33 * b00,
                                     a21 * b02 - a20 * b04 - a23 * b00,
                                     a11 * b07 - a10 * b09 - a12 * b06,
                                     a00 * b09 - a01 * b07 + a02 * b06,
                                     a31 * b01 - a30 * b03 - a32 * b00,
                                     a20 * b03 - a21 * b01 + a22 * b00) / det;
                }
                void main () {
                    vec2 position = ( gl_FragCoord.xy / resolution.xy );
                    float max_z = texture2D(depthBuffer, position).z;
                    vec4 texel =  texture2D(texture, gl_TexCoord[0].xy);
                    vec4 colors[2];
                    colors[1] = texel * gl_Color;
                    colors[0] = gl_Color;
                    bool b = (haveTexture > 0.9);
                    vec4 color = colors[int(b)];
                    float z = (gl_FragCoord.w != 1.f) ? (inverse(projMat) * vec4(0, 0, 0, gl_FragCoord.w)).w : gl_FragCoord.z;
                    colors[1] = color;
                    colors[0] = vec4(0, 0, 0, 0);
                    float epsilon =  0.0001;
                    b = (z /*+ epsilon*/ < max_z);
                    gl_FragColor = colors[int(b)];
                })";
                const std::string layersCountFragShader =
                R"(#version 140
                #extension GL_ARB_shader_atomic_counters : require
                #extension GL_ARB_shading_language_420pack : require
                uniform float maxLayers;
                layout( binding = 0) uniform atomic_uint counter;
                void main () {
                    float val = atomicCounterIncrement(counter) / maxLayers;
                    gl_FragColor = vec4(val, val, val, 1);
                })";
                const std::string copyDepthBufferFragShader =
                R"(#version 130
                uniform sampler2D depthTexture;
                void main() {
                   gl_FragColor = texture2D(depthTexture, gl_TexCoord[0].xy);
                })";
                const std::string depthPeelingGenFragShader =
                R"(#version 130
                in mat4 projMat;
                uniform sampler2D depthTexture;
                mat4 inverse(mat4 m) {
                         float
                         a00 = m[0][0], a01 = m[0][1], a02 = m[0][2], a03 = m[0][3],
                         a10 = m[1][0], a11 = m[1][1], a12 = m[1][2], a13 = m[1][3],
                         a20 = m[2][0], a21 = m[2][1], a22 = m[2][2], a23 = m[2][3],
                         a30 = m[3][0], a31 = m[3][1], a32 = m[3][2], a33 = m[3][3],
                         b00 = a00 * a11 - a01 * a10,
                         b01 = a00 * a12 - a02 * a10,
                         b02 = a00 * a13 - a03 * a10,
                         b03 = a01 * a12 - a02 * a11,
                         b04 = a01 * a13 - a03 * a11,
                         b05 = a02 * a13 - a03 * a12,
                         b06 = a20 * a31 - a21 * a30,
                         b07 = a20 * a32 - a22 * a30,
                         b08 = a20 * a33 - a23 * a30,
                         b09 = a21 * a32 - a22 * a31,
                         b10 = a21 * a33 - a23 * a31,
                         b11 = a22 * a33 - a23 * a32,
                         det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;
                         return mat4(
                                     a11 * b11 - a12 * b10 + a13 * b09,
                                     a02 * b10 - a01 * b11 - a03 * b09,
                                     a31 * b05 - a32 * b04 + a33 * b03,
                                     a22 * b04 - a21 * b05 - a23 * b03,
                                     a12 * b08 - a10 * b11 - a13 * b07,
                                     a00 * b11 - a02 * b08 + a03 * b07,
                                     a32 * b02 - a30 * b05 - a33 * b01,
                                     a20 * b05 - a22 * b02 + a23 * b01,
                                     a10 * b10 - a11 * b08 + a13 * b06,
                                     a01 * b08 - a00 * b10 - a03 * b06,
                                     a30 * b04 - a31 * b02 + a33 * b00,
                                     a21 * b02 - a20 * b04 - a23 * b00,
                                     a11 * b07 - a10 * b09 - a12 * b06,
                                     a00 * b09 - a01 * b07 + a02 * b06,
                                     a31 * b01 - a30 * b03 - a32 * b00,
                                     a20 * b03 - a21 * b01 + a22 * b00) / det;
                }
                void main () {
                    float max_z = texture2D(depthTexture, gl_TexCoord[0].xy).z;
                    float z = (gl_FragCoord.w != 1.f) ? (inverse(projMat) * vec4(0, 0, 0, gl_FragCoord.w)).w : gl_FragCoord.z;
                    vec4 colors[2];
                    colors[0] = vec4(0, 0, 0, 0);
                    colors[1] = vec4(0, 0, z, 1);
                    float epsilon = 0.0001;
                    bool b = (z /*+ epsilon*/ < max_z);
                    gl_FragColor = colors[int(b)];
                })";
                if (!depthBufferGenerator.loadFromMemory(vertexShader, depthGenFragShader))
                    throw core::Erreur(50, "Failed to load depth buffer generator shader", 0);
                if (!frameBufferGenerator.loadFromMemory(vertexShader, frameBufferGenFragShader))
                    throw core::Erreur(51, "Failed to load frame buffer generator shader", 0);
                if (!simpleShader.loadFromMemory(vertexShader, frameBufferGenFragShader))
                    throw core::Erreur(52, "Failed to load simple shader", 0);
                if (!depthPeelingGenerator.loadFromMemory(vertexShader,depthPeelingGenFragShader))
                    throw core::Erreur(53, "Failed to load depth peeling shader", 0);
                if (!transformFragShader.loadFromMemory(transfFragVertexShader, transfFragFragmentShader)) {
                    throw core::Erreur(54, "Failed to load transform frag shader");
                }
                if (!countNbLayers.loadFromMemory(simpleVertexShader, layersCountFragShader)) {
                    throw core::Erreur(54, "Failed to load count nb layers shader");
                }
                 if (!copyDepthBuffer.loadFromMemory(simpleVertexShader, copyDepthBufferFragShader)) {
                    throw core::Erreur(54, "Failed to load copy depth buffer shader");
                }
                //throw core::Erreur(52, "Failed to load specular texture generator shader", 0);
                frameBufferGenerator.setParameter("resolution",resolution.x, resolution.y, resolution.z);
                frameBufferGenerator.setParameter("depthBuffer", depthBuffer.getTexture());
                frameBufferGenerator.setParameter("texture", Shader::CurrentTexture);
                depthBufferGenerator.setParameter("texture", Shader::CurrentTexture);
                depthPeelingGenerator.setParameter("depthTexture", Shader::CurrentTexture);
                simpleShader.setParameter("texture",Shader::CurrentTexture);
                transformFragShader.setParameter("resolution", resolution.x, resolution.y, resolution.z);
                transformFragShader.setParameter("oldFrameBuffer", previousFrame.getTexture());
                copyDepthBuffer.setParameter("depthTexture", Shader::CurrentTexture);
                countNbLayers.setParameter("maxLayers", MAX_LAYERS);
                //specularTextureGenerator->setParameter("specularTexture",specularTexture->getTexture());
                backgroundColor = sf::Color::Transparent;
                glCheck(glGenBuffers(2, pboIds));
                glCheck(glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[0]));
                glCheck(glBufferData(GL_PIXEL_PACK_BUFFER, view.getSize().x * view.getSize().y * 4, 0, GL_STREAM_READ));
                glCheck(glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[1]));
                glCheck(glBufferData(GL_PIXEL_PACK_BUFFER, view.getSize().x * view.getSize().y * 4, 0, GL_STREAM_READ));
                glCheck(glBindBuffer(GL_PIXEL_PACK_BUFFER, 0));
                // declare and generate a buffer object name

            } else {
                throw core::Erreur(55, "Shader not supported!", 0);
            }
        }
        void DepthPeelingRenderComponent::pushEvent(window::IEvent event, RenderWindow& rw) {
            if (event.type == window::IEvent::WINDOW_EVENT && event.window.type == window::IEvent::WINDOW_EVENT_RESIZED && &getWindow() == &rw && isAutoResized()) {
                std::cout<<"recompute size"<<std::endl;
                recomputeSize();
                getListener().pushEvent(event);
                getView().reset(physic::BoundingBox(getView().getViewport().getPosition().x, getView().getViewport().getPosition().y, getView().getViewport().getPosition().z, event.window.data1, event.window.data2, getView().getViewport().getDepth()));
            }
        }
        bool DepthPeelingRenderComponent::needToUpdate() {
            return update;
        }
        void DepthPeelingRenderComponent::changeVisibleEntities(Entity* toRemove, Entity* toAdd, EntityManager* em) {
            bool removed;
            em->removeAnimatedVisibleEntity(toRemove, visibleEntities, view, removed);
            if (removed) {
                em->insertAnimatedVisibleEntity(toAdd, visibleEntities, view);
                loadEntitiesOnComponent(visibleEntities);
                update = true;
                animationUpdated = true;
            }
        }
        std::string DepthPeelingRenderComponent::getExpression() {
            return expression;
        }
        void DepthPeelingRenderComponent::setBackgroundColor(sf::Color color) {
            this->backgroundColor = color;
        }
        void DepthPeelingRenderComponent::clear() {
             frameBuffer.clear(backgroundColor);
             depthBuffer.clear(sf::Color::Transparent);
             frontBuffer.clear(sf::Color::Transparent);
             copiedDepthBuffer.clear(sf::Color::Transparent);
             nbLayersCount.clear(sf::Color::Transparent);
             GLuint zero = 0;
             glCheck(glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, atomicsBuffer));
             glCheck(glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &zero));


        }
        Sprite& DepthPeelingRenderComponent::getFrameBufferSprite () {
            return frameBufferSprite;
        }
        Sprite& DepthPeelingRenderComponent::getDepthBufferSprite() {
            return depthBufferSprite;
        }
        Sprite& DepthPeelingRenderComponent::getCopiedDepthBufferSprite() {
            return copiedDepthBufferSprite;
        }
        Sprite& DepthPeelingRenderComponent::getLayersCountSprite() {
            return layersCountSprite;
        }
        Sprite& DepthPeelingRenderComponent::getFrontBufferSprite() {
            return frontBufferSprite;
        }
        const Texture& DepthPeelingRenderComponent::getDepthBufferTexture() {
            return depthBuffer.getTexture();
        }
        const Texture& DepthPeelingRenderComponent::getFrameBufferTexture() {
            return frameBuffer.getTexture();
        }
        const Texture& DepthPeelingRenderComponent::getFrontBufferTexture() {
            return frontBuffer.getTexture();
        }
        bool DepthPeelingRenderComponent::loadEntitiesOnComponent(std::vector<Entity*> vEntities)
        {
            //if (firstDraw) {
                /*I tried to redraw the previous frame by transforming fragments when the view is updated to compute the
                transparency for new objects (which are in the updated view) only to reduce the
                number of layers when performing the depthpeeling (otherwise it's too slow) but my shader doesn't work.
            */
                m_newInstances.clear();
                physic::BoundingBox bbox = view.getViewVolume();
                std::map<physic::BoundingBox, std::vector<Entity*>> newEntities;
                newEntities.insert(std::make_pair(bbox, vEntities));
                visibleEntities.clear();
                insertNewEntities(newEntities);
                update = true;
            /*} else if (viewUpdated) {
                m_newInstances.clear();
                visibleEntities.clear();
                batcher.clear();
                physic::BoundingBox viewArea = view.getViewVolume();
                viewArea.setSize(viewArea.getSize().x, viewArea.getSize().y + 100, viewArea.getSize().z);
                physic::BoundingBox oldViewArea = previousView.getViewVolume();
                oldViewArea.setSize(oldViewArea.getSize().x, oldViewArea.getSize().y, oldViewArea.getSize().z);
                std::vector<physic::BoundingBox> areas;
                if (viewArea.getPosition().x < oldViewArea.getPosition().x && viewArea.getPosition().y < oldViewArea.getPosition().y) {
                    int a1PX = viewArea.getPosition().x + (oldViewArea.getPosition().x - viewArea.getPosition().x);
                    int a1PY = viewArea.getPosition().y;
                    int a1SX = viewArea.getSize().x - (oldViewArea.getPosition().x - viewArea.getPosition().x);
                    int a1SY = oldViewArea.getPosition().y - viewArea.getPosition().y;
                    int a2PX = viewArea.getPosition().x;
                    int a2PY = viewArea.getPosition().y;
                    int a2SX = oldViewArea.getPosition().x - viewArea.getPosition().x;
                    int a2SY = viewArea.getSize().y;
                    physic::BoundingBox a1 (a1PX, a1PY, viewArea.getPosition().z, a1SX, a1SY, viewArea.getSize().z);
                    physic::BoundingBox a2 (a2PX, a2PY, viewArea.getPosition().z, a2SX, a2SY, viewArea.getSize().z);
                    areas.push_back(a1);
                    areas.push_back(a2);
                } else if (viewArea.getPosition().x == oldViewArea.getPosition().x && viewArea.getPosition().y > oldViewArea.getPosition().y) {
                    int a1PX = viewArea.getPosition().x;
                    int a1PY = oldViewArea.getPosition().y + oldViewArea.getSize().y;
                    int a1SX = viewArea.getSize().x;
                    int a1SY = viewArea.getPosition().y - oldViewArea.getPosition().y;
                    physic::BoundingBox a1 (a1PX, a1PY, viewArea.getPosition().z, a1SX, a1SY, viewArea.getSize().z);
                    //std::cout<<"a1 : "<<a1.getPosition()<<a1.getSize();
                    areas.push_back(a1);
                } else if (viewArea.getPosition().x > oldViewArea.getPosition().x && viewArea.getPosition().y < oldViewArea.getPosition().y) {
                    int a1PX = viewArea.getPosition().x;
                    int a1PY = viewArea.getPosition().y;
                    int a1SX = viewArea.getSize().x - (viewArea.getPosition().x - oldViewArea.getPosition().x);
                    int a1SY = oldViewArea.getPosition().y - viewArea.getPosition().y;
                    int a2PX = viewArea.getPosition().x + viewArea.getSize().x - (view.getPosition().x - oldViewArea.getPosition().x);
                    int a2PY = viewArea.getPosition().y;
                    int a2SX = viewArea.getPosition().x - oldViewArea.getPosition().x;
                    int a2SY = viewArea.getSize().y - (oldViewArea.getPosition().y - viewArea.getPosition().y);
                    physic::BoundingBox a1 (a1PX, a1PY, viewArea.getPosition().z, a1SX, a1SY, viewArea.getSize().z);
                    physic::BoundingBox a2 (a2PX, a2PY, viewArea.getPosition().z, a2SX, a2SY, viewArea.getSize().z);
                    areas.push_back(a1);
                    areas.push_back(a2);
                } else if (viewArea.getPosition().x == oldViewArea.getPosition().x && viewArea.getPosition().y < oldViewArea.getPosition().y) {
                    int a1PX = viewArea.getPosition().x;
                    int a1PY = viewArea.getPosition().y;
                    int a1SX = viewArea.getSize().x;
                    int a1SY = oldViewArea.getPosition().y - viewArea.getPosition().y;
                    physic::BoundingBox a1 (a1PX, a1PY, viewArea.getPosition().z, a1SX, a1SY, viewArea.getSize().z);
                    areas.push_back(a1);
                } else if (viewArea.getPosition().x < oldViewArea.getPosition().x && viewArea.getPosition().y > oldViewArea.getPosition().y) {
                    int a1PX = viewArea.getPosition().x;
                    int a1PY = viewArea.getPosition().y;
                    int a1SX = oldViewArea.getPosition().x - viewArea.getPosition().x;
                    int a1SY = viewArea.getSize().y;
                    int a2PX = oldViewArea.getPosition().x;
                    int a2PY = oldViewArea.getPosition().y + oldViewArea.getSize().y;
                    int a2SX = viewArea.getSize().x - (oldViewArea.getPosition().x - viewArea.getPosition().x);
                    int a2SY = view.getPosition().y - oldViewArea.getPosition().y;
                    physic::BoundingBox a1 (a1PX, a1PY, viewArea.getPosition().z, a1SX, a1SY, viewArea.getSize().z);
                    physic::BoundingBox a2 (a2PX, a2PY, viewArea.getPosition().z, a2SX, a2SY, viewArea.getSize().z);
                    areas.push_back(a1);
                    areas.push_back(a2);
                } else if (viewArea.getPosition().x < oldViewArea.getPosition().x && viewArea.getPosition().y == oldViewArea.getPosition().y) {
                    int a1PX = viewArea.getPosition().x;
                    int a1PY = viewArea.getPosition().y;
                    int a1SX = oldViewArea.getPosition().x - viewArea.getPosition().x;
                    int a1SY = viewArea.getSize().y;
                    physic::BoundingBox a1 (a1PX, a1PY, viewArea.getPosition().z, a1SX, a1SY, viewArea.getSize().z);
                    areas.push_back(a1);
                } else if (viewArea.getPosition().x > oldViewArea.getPosition().x && viewArea.getPosition().y > oldViewArea.getPosition().y) {
                    int a1PX = viewArea.getPosition().x + viewArea.getSize().x - (viewArea.getPosition().x - oldViewArea.getPosition().x);
                    int a1PY = viewArea.getPosition().y;
                    int a1SX = viewArea.getPosition().x - oldViewArea.getPosition().x;
                    int a1SY = viewArea.getSize().y - (viewArea.getPosition().y - oldViewArea.getPosition().y);
                    int a2PX = viewArea.getPosition().x;
                    int a2PY = oldViewArea.getPosition().y - (viewArea.getPosition().y - oldViewArea.getPosition().y);
                    int a2SX = viewArea.getPosition().x;
                    int a2SY = viewArea.getPosition().y - oldViewArea.getPosition().y;
                    physic::BoundingBox a1 (a1PX, a1PY, viewArea.getPosition().z, a1SX, a1SY, viewArea.getSize().z);
                    physic::BoundingBox a2 (a2PX, a2PY, viewArea.getPosition().z, a2SX, a2SY, viewArea.getSize().z);
                    areas.push_back(a1);
                    areas.push_back(a2);
                } else if (viewArea.getPosition().x > oldViewArea.getPosition().x && viewArea.getPosition().y == oldViewArea.getPosition().y) {
                    int a1PX = viewArea.getPosition().x;
                    int a1PY = viewArea.getPosition().y;
                    int a1SX = viewArea.getSize().x - (viewArea.getPosition().x - oldViewArea.getPosition().x);
                    int a1SY = viewArea.getSize().y;
                    physic::BoundingBox a1 (a1PX, a1PY, viewArea.getPosition().z, a1SX, a1SY, viewArea.getSize().z);
                    areas.push_back(a1);
                }
                std::map<physic::BoundingBox, std::vector<Entity*>> newEntities;
                for (unsigned int i = 0; i < areas.size(); i++) {
                    newEntities.insert(std::make_pair(areas[i], std::vector<Entity*>()));
                }
                for (unsigned int i = 0; i < vEntities.size(); i++) {
                    physic::BoundingBox bounds = vEntities[i]->getGlobalBounds();
                    if (bounds.intersects(oldViewArea)) {
                        for (unsigned int j = 0; j < vEntities[i]->getFaces().size(); j++) {
                            batcher.addFace(vEntities[i]->getFaces()[j]);
                        }
                        visibleEntities.push_back(vEntities[i]);
                    }
                    std::map<physic::BoundingBox, std::vector<Entity*>>::iterator it;
                    for (unsigned int j = 0; j < areas.size(); j++) {
                        if (areas[j].intersects(bounds)) {
                            //std::cout<<"intersects"<<std::endl;
                            it = newEntities.find(areas[j]);
                            it->second.push_back(vEntities[i]);
                        }
                    }
                }
                m_instances = batcher.getInstances();
                insertNewEntities(newEntities);
            }*/
            return true;
        }
        void DepthPeelingRenderComponent::setView(View view) {
            previousView = this->view;
            this->view = view;
            frameBuffer.setView(view);
            depthBuffer.setView(view);
            frontBuffer.setView(view);
            previousFrame.setView(view);
            copiedDepthBuffer.setView(view);
            nbLayersCount.setView(view);
            viewUpdated = true;
        }
        void DepthPeelingRenderComponent::setExpression(std::string expression) {
            update = true;
            this->expression = expression;
        }
        void DepthPeelingRenderComponent::drawNextFrame() {
            update = false;
            /*TransformFragShader doesn't work."*/
            /*if (!firstDraw && viewUpdated) {
                transformFragShader.setParameter("oldViewMatrix", previousView.getViewMatrix().getMatrix().transpose());
                ViewportMatrix vpm;
                vpm.setViewport(previousView.getViewport().getPosition(), previousView.getViewport().getSize());
                transformFragShader.setParameter("viewportMatrix", vpm.getMatrix().transpose());
                math::Vec3f offsets = view.getPosition() - previousView.getPosition();
                //Strange things happens at the texture's borders, when the view moves right or top, pixels are not transparent
                //when the pixel position is greater than the screen resolution.
                transformFragShader.setParameter("uoffsets", offsets.x, offsets.y);
                currentStates.blendMode = BlendNone;
                currentStates.shader = &transformFragShader;
                for (unsigned int i = 0; i < m_instances.size(); i++) {
                    physic::BoundingBox bbox = previousView.getViewVolume();
                    math::Vec3f bbScreenPos = frameBuffer.mapCoordsToPixel(bbox.getPosition(), view);
                    frameBuffer.setActive();
                    glCheck(glEnable(GL_SCISSOR_TEST));
                    glCheck(glScissor(bbScreenPos.x, bbox.getSize().y - (bbScreenPos.y + bbox.getSize().y),bbox.getSize().x, bbox.getSize().y));
                    frameBuffer.draw(m_instances[i].getAllVertices(), currentStates);
                    glCheck(glDisable(GL_SCISSOR_TEST));
                }
                frameBuffer.display();
            }*/
            /*Need to draw the scene twice at the very first draw call because at the first call the depthtest (on the depth texture) never pass and it doesn't draw anything.
            this is very strange.*/
           // if (viewUpdated || firstDraw || secondDraw || animationUpdated) {
                currentStates.blendMode = sf::BlendNone;
                /*glCheck(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicsBuffer));
                GLuint* ptr = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint),
                                                        GL_MAP_WRITE_BIT |
                                                        GL_MAP_INVALIDATE_BUFFER_BIT |
                                                        GL_MAP_UNSYNCHRONIZED_BIT);
                memset(ptr, 0, sizeof(GLuint));
                glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
                glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);*/

                currentStates.shader = &countNbLayers;
                std::map<physic::BoundingBox, std::pair<unsigned int, std::vector<Instance>>>::iterator it;
                for (it = m_newInstances.begin(); it != m_newInstances.end(); it++) {
                    for (unsigned int i = 0; i < it->second.second.size(); i++) {
                        nbLayersCount.draw(it->second.second[i].getAllVertices(),currentStates);
                    }
                }
                nbLayersCount.display();
                glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

                glCheck(glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[0]));
                Texture::bind(&nbLayersCount.getTexture());
                glCheck(glGetTexImage(GL_TEXTURE_2D,
                  0,
                  GL_RGBA,
                  GL_UNSIGNED_BYTE,
                  nullptr));
                GLsync sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
                GLint result;
                glCheck(glGetSynciv(sync, GL_SYNC_STATUS, sizeof(result), NULL, &result));
                if(result == GL_SIGNALED){
                    nbLayers = 0;
                    glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[0]);
                    GLubyte* src = (GLubyte*) glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
                    if(src) {

                        for (unsigned int i = 0; i < view.getSize().x * view.getSize().y; i++) {
                            unsigned int val = src[i*4] / 255.f  * MAX_LAYERS;
                            nbLayers = (val > nbLayers) ? val : nbLayers;

                        }
                        glCheck(glUnmapBuffer(GL_PIXEL_PACK_BUFFER));
                    }
                }
                glCheck(glBindBuffer(GL_PIXEL_PACK_BUFFER, 0));
                std::cout<<"nb layers : "<<nbLayers<<std::endl;
                currentStates.blendMode = BlendNone;
                //std::map<physic::BoundingBox, std::pair<unsigned int, std::vector<Instance>>>::iterator it;
                for (it = m_newInstances.begin(); it != m_newInstances.end(); it++) {
                    for (unsigned int i = 0; i < it->second.second.size(); i++) {
                         if (it->second.second[i].getAllVertices().getVertexCount() > 0) {
                             if (it->second.second[i].getMaterial().getTexture() == nullptr) {
                                depthBufferGenerator.setParameter("haveTexture", 0.f);
                                simpleShader.setParameter("haveTexture", 0.f);
                             } else {
                                depthBufferGenerator.setParameter("haveTexture", 1.f);
                                simpleShader.setParameter("haveTexture", 1.f);
                             }
                             currentStates.shader=&depthBufferGenerator;
                             currentStates.texture=it->second.second[i].getMaterial().getTexture();
                             physic::BoundingBox bbox = it->first;
                             math::Vec3f bbScreenPos = depthBuffer.mapCoordsToPixel(bbox.getPosition(), view);
                             depthBuffer.draw(it->second.second[i].getAllVertices(),currentStates);
                             //glCheck(glDisable(GL_SCISSOR_TEST));
                             currentStates.shader = nullptr;
                             frontBuffer.draw(it->second.second[i].getAllVertices(), currentStates);
                             //glCheck(glDisable(GL_SCISSOR_TEST));
                        }
                    }
                }
                frontBuffer.display();
                depthBuffer.display();
                frontBufferSprite.setCenter(view.getPosition());
                currentStates.blendMode = sf::BlendAlpha;
                frameBuffer.draw(frontBufferSprite, currentStates);

                for (it = m_newInstances.begin(); it != m_newInstances.end(); it++) {
                    for (unsigned int l = 0; l < nbLayers; l++) {

                            for (unsigned int i = 0; i < it->second.second.size(); i++) {
                                if (it->second.second[i].getAllVertices().getVertexCount() > 0) {
                                    currentStates.shader = &frameBufferGenerator;
                                    currentStates.blendMode = sf::BlendNone;
                                    frontBuffer.clear(sf::Color::Transparent);
                                     if (it->second.second[i].getMaterial().getTexture() == nullptr) {
                                        frameBufferGenerator.setParameter("haveTexture", 0.f);
                                     } else {
                                        frameBufferGenerator.setParameter("haveTexture", 1.f);
                                     }
                                     currentStates.texture=it->second.second[i].getMaterial().getTexture();
                                     physic::BoundingBox bbox = it->first;
                                     math::Vec3f bbScreenPos = frontBuffer.mapCoordsToPixel(bbox.getPosition(), view);
                                     frontBuffer.draw(it->second.second[i].getAllVertices(), currentStates);
                                }
                                 //glCheck(glDisable(GL_SCISSOR_TEST));
                            }
                            frontBuffer.display();
                            frontBufferSprite.setCenter(view.getPosition());
                            copiedDepthBuffer.clear(sf::Color::Transparent);
                            currentStates.shader = nullptr;
                            currentStates.blendMode = sf::BlendNone;
                            currentStates.shader = &copyDepthBuffer;
                            currentStates.texture = &depthBuffer.getTexture();
                            for (unsigned int i = 0; i < it->second.second.size(); i++) {
                                if (it->second.second[i].getAllVertices().getVertexCount() > 0) {
                                    copiedDepthBuffer.draw(it->second.second[i].getAllVertices(), currentStates);
                                }
                            }
                            depthBuffer.clear(sf::Color::Transparent);
                            currentStates.shader = &depthPeelingGenerator;
                            currentStates.texture = &copiedDepthBuffer.getTexture();
                            for (unsigned int i = 0; i < it->second.second.size(); i++) {
                                if (it->second.second[i].getAllVertices().getVertexCount() > 0) {
                                     physic::BoundingBox bbox = it->first;
                                     math::Vec3f bbScreenPos = frontBuffer.mapCoordsToPixel(bbox.getPosition(), view);
                                     depthBuffer.draw(it->second.second[i].getAllVertices(), currentStates);
                                }
                                 //glCheck(glDisable(GL_SCISSOR_TEST));
                            }
                            depthBuffer.display();
                            copiedDepthBuffer.display();
                            currentStates.shader = nullptr;
                            currentStates.blendMode = sf::BlendMode(sf::BlendMode::Factor::OneMinusDstAlpha, sf::BlendMode::Factor::DstAlpha, sf::BlendMode::Add, sf::BlendMode::Factor::OneMinusDstAlpha, sf::BlendMode::Factor::One, sf::BlendMode::Equation::Add);
                            frameBuffer.draw(frontBufferSprite, currentStates);
                    }
                }
                for (unsigned int i = 0; i < drawables.size(); i++) {
                    frameBuffer.draw(drawables[i].first.get(), drawables[i].second);
                }
                frameBuffer.display();
                /*previousFrame.clear(backgroundColor);
                currentStates.blendMode = sf::BlendNone;
                currentStates.shader = nullptr;
                frameBufferSprite.setCenter(view.getPosition());
                previousFrame.draw(frameBufferSprite, currentStates);
                previousFrame.display();
                previousFrameBufferSprite.setCenter(view.getPosition());
                if (firstDraw)
                    secondDraw = true;
                if (!firstDraw)
                    secondDraw = false;
                firstDraw = false;
                viewUpdated = false;
            } else {
                currentStates.blendMode = sf::BlendNone;
                currentStates.shader = nullptr;
                frameBuffer.draw(previousFrameBufferSprite, currentStates);
                frameBuffer.display();
            }*/
        }
       std::vector<Entity*> DepthPeelingRenderComponent::getEntities() {
            return visibleEntities;
        }
        void DepthPeelingRenderComponent::draw(Drawable& drawable, RenderStates states) {
            update = true;
            drawables.push_back(std::make_pair(std::ref(drawable), states));
        }
        void DepthPeelingRenderComponent::draw(RenderTarget& target, RenderStates states) {
            frameBufferSprite.setCenter(target.getView().getPosition());
            target.draw(frameBufferSprite, states);
            drawables.clear();
        }

        View& DepthPeelingRenderComponent::getView() {
            return view;
        }
        int DepthPeelingRenderComponent::getLayer() {
            return getPosition().z;
        }
        void DepthPeelingRenderComponent::removeEntities(physic::BoundingBox box) {
            std::vector<Entity*>::iterator it;
            for (it = visibleEntities.begin(); it != visibleEntities.end();) {
                physic::BoundingBox bounds = (*it)->getGlobalBounds();
                if (bounds.intersects(box)) {
                    it = visibleEntities.erase(it);
                } else {
                    it++;
                }
            }
        }
        void DepthPeelingRenderComponent::insertNewEntities(std::map<physic::BoundingBox, std::vector<Entity*>> vEntities) {
            std::map<physic::BoundingBox, std::vector<Entity*>>::iterator it;
            for (it = vEntities.begin(); it != vEntities.end(); it++) {
                newBatcher.clear();
                for (unsigned int i = 0; i < it->second.size(); i++) {
                    if ( it->second[i]->isLeaf()) {
                        for (unsigned int j = 0; j <  it->second[i]->getFaces().size(); j++) {
                             newBatcher.addFace(it->second[i]->getFaces()[j]);
                        }
                    }
                    visibleEntities.push_back(it->second[i]);
                }
                m_newInstances.insert(std::make_pair(it->first, std::make_pair(newBatcher.getNbLayers(), newBatcher.getInstances())));
            }
            update = true;
        }
        void DepthPeelingRenderComponent::updateParticleSystems() {
            for (unsigned int i = 0; i < visibleEntities.size(); i++) {
                if (dynamic_cast<physic::ParticleSystem*>(visibleEntities[i]) != nullptr) {
                    static_cast<physic::ParticleSystem*>(visibleEntities[i])->update();
                }
            }
            loadEntitiesOnComponent(visibleEntities);
            update = true;
            animationUpdated = true;
        }
        DepthPeelingRenderComponent::~DepthPeelingRenderComponent() {
            glDeleteBuffers(2, pboIds);
            glDeleteBuffers(1, &atomicsBuffer);
        }
    }
}
