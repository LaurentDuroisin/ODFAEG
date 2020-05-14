#include <GL/glew.h>
#include "../../../include/odfaeg/Graphics/lightRenderComponent.hpp"
#include "glCheck.h"
#include <memory.h>
using namespace sf;
using namespace std;
namespace odfaeg {
    namespace graphic {
        LightRenderComponent::LightRenderComponent (RenderWindow& window, int layer, std::string expression,window::ContextSettings settings) :
                    HeavyComponent(window, math::Vec3f(window.getView().getPosition().x, window.getView().getPosition().y, layer),
                                  math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0),
                                  math::Vec3f(window.getView().getSize().x + window.getView().getSize().x * 0.5f, window.getView().getPosition().y + window.getView().getSize().y * 0.5f, layer)),
                    view(window.getView()),
                    expression(expression) {
                    update = false;
                    sf::Vector3i resolution ((int) window.getSize().x, (int) window.getSize().y, window.getView().getSize().z);
                    settings.depthBits = 24;
                    depthBuffer.create(resolution.x, resolution.y,settings);
                    specularTexture.create(resolution.x, resolution.y,settings);
                    bumpTexture.create(resolution.x, resolution.y,settings);
                    settings.depthBits = 0;
                    lightMap.create(resolution.x, resolution.y,settings);
                    normalMap.create(resolution.x, resolution.y,settings);
                    normalMap.setView(window.getView());
                    depthBuffer.setView(window.getView());
                    specularTexture.setView(window.getView());
                    bumpTexture.setView(window.getView());
                    lightMap.setView(window.getView());
                    lightMapTile = Sprite(lightMap.getTexture(), math::Vec3f(0, 0, 0), math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0), IntRect(0, 0, window.getView().getSize().x, window.getView().getSize().y));
                    depthBufferTile = Sprite(depthBuffer.getTexture(), math::Vec3f(0, 0, 0), math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0), IntRect(0, 0, window.getView().getSize().x, window.getView().getSize().y));
                    normalMapTile = Sprite(normalMap.getTexture(), math::Vec3f(0, 0, 0), math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0), IntRect(0, 0, window.getView().getSize().x, window.getView().getSize().y));
                    specularBufferTile = Sprite(specularTexture.getTexture(), math::Vec3f(0, 0, 0), math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0), IntRect(0, 0, window.getView().getSize().x, window.getView().getSize().y));
                    bumpMapTile = Sprite(bumpTexture.getTexture(), math::Vec3f(0, 0, 0), math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0), IntRect(0, 0, window.getView().getSize().x, window.getView().getSize().y));
                    core::FastDelegate<bool> signal (&LightRenderComponent::needToUpdate, this);
                    core::FastDelegate<void> slot (&LightRenderComponent::drawNextFrame, this);
                    core::Command cmd(signal, slot);
                    getListener().connect("UPDATE", cmd);
                    if (settings.versionMajor >= 3 && settings.versionMinor >= 3) {
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
                        const std::string buildNormalMapVertexShader = R"(#version 460 core
                                                                    layout (location = 0) in vec3 position;
                                                                    layout (location = 1) in vec4 color;
                                                                    layout (location = 2) in vec2 texCoords;
                                                                    uniform mat4 projectionMatrix;
                                                                    uniform mat4 viewMatrix;
                                                                    uniform mat4 worldMatrix;
                                                                    uniform mat4 textureMatrix;
                                                                    out vec2 fTexCoords;
                                                                    out vec4 frontColor;
                                                                    void main() {
                                                                        gl_Position = projectionMatrix * viewMatrix * worldMatrix * vec4(position, 1.f);
                                                                        fTexCoords = (textureMatrix * vec4(texCoords, 1.f, 1.f)).xy;
                                                                        frontColor = color;
                                                                    }
                                                                    )";
                        const std::string buildNormalMapFragmentShader = R"(#version 460 core
                                                                            in vec4 frontColor;
                                                                            in vec2 fTexCoords;
                                                                            const vec2 size = vec2(2.0, 0.0);
                                                                            const ivec3 off = ivec3(-1, 0, 1);
                                                                            uniform sampler2D texture;
                                                                            layout (location = 0) out vec4 fColor;
                                                                            void main () {
                                                                                vec4 depth = texture2D(texture, fTexCoords);
                                                                                float s01 = textureOffset(texture, fTexCoords, off.xy).z;
                                                                                float s21 = textureOffset(texture, fTexCoords, off.zy).z;
                                                                                float s10 = textureOffset(texture, fTexCoords, off.yx).z;
                                                                                float s12 = textureOffset(texture, fTexCoords, off.yz).z;
                                                                                vec3 va = normalize (vec3(size.xy, s21 - s01));
                                                                                vec3 vb = normalize (vec3(size.yx, s12 - s10));
                                                                                fColor = vec4(cross(va, vb), depth.z);
                                                                            }
                                                                            )";
                        const std::string depthGenFragShader = R"(#version 460 core
                                                                  in vec4 frontColor;
                                                                  in vec2 fTexCoords;
                                                                  uniform sampler2D texture;
                                                                  uniform float haveTexture;
                                                                  layout (location = 0) out vec4 fColor;
                                                                  void main () {
                                                                      vec4 texel = texture2D(texture, fTexCoords);
                                                                      vec4 colors[2];
                                                                      colors[1] = texel * frontColor;
                                                                      colors[0] = frontColor;
                                                                      bool b = (haveTexture > 0.9);
                                                                      float current_alpha = colors[int(b)].a;
                                                                      float z = gl_FragCoord.z;
                                                                      fColor = vec4(0, 0, z, current_alpha);
                                                                  }
                                                                  )";
                        const std::string specularGenFragShader = R"(#version 460 core
                                                                     in vec4 frontColor;
                                                                     in vec2 fTexCoords;
                                                                     uniform sampler2D texture;
                                                                     uniform float maxM;
                                                                     uniform float maxP;
                                                                     uniform float m;
                                                                     uniform float p;
                                                                     uniform float haveTexture;
                                                                     layout (location = 0) out vec4 fColor;
                                                                     void main() {
                                                                        vec4 texel = texture2D(texture, fTexCoords);
                                                                        vec4 colors[2];
                                                                        colors[1] = texel * frontColor;
                                                                        colors[0] = frontColor;
                                                                        bool b = (haveTexture > 0.9);
                                                                        vec4 color = colors[int(b)];
                                                                        float z = gl_FragCoord.z;
                                                                        float intensity = (maxM != 0.f) ? m / maxM : 0.f;
                                                                        float power = (maxP != 0.f) ? p / maxP : 0.f;
                                                                        fColor = vec4(intensity, power, z, color.a);
                                                                     }
                                                                  )";
                        const std::string bumpGenFragShader =    R"(#version 460 core
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
                                                                     bool b = (haveTexture > 0.9);
                                                                     vec4 color = colors[int(b)];
                                                                     fColor = color;
                                                                 }
                                                                 )";
                        const std::string perPixLightingFragmentShader =
                                                                 R"(#version 460 core
                                                                 in vec4 frontColor;
                                                                 in vec2 fTexCoords;
                                                                 const vec2 size = vec2(2.0,0.0);
                                                                 const ivec3 off = ivec3(-1,0,1);
                                                                 uniform sampler2D depthTexture;
                                                                 uniform sampler2D lightMap;
                                                                 uniform sampler2D specularTexture;
                                                                 uniform sampler2D bumpMap;
                                                                 uniform vec3 resolution;
                                                                 uniform vec4 lightColor;
                                                                 uniform vec4 lightPos;
                                                                 layout (location = 0) out vec4 fColor;
                                                                 void main () {
                                                                     vec2 position = (gl_FragCoord.xy / resolution.xy);
                                                                     vec4 depth = texture2D(depthTexture, position);
                                                                     float s01 = textureOffset(depthTexture, position, off.xy).z;
                                                                     float s21 = textureOffset(depthTexture, position, off.zy).z;
                                                                     float s10 = textureOffset(depthTexture, position, off.yx).z;
                                                                     float s12 = textureOffset(depthTexture, position, off.yz).z;
                                                                     vec3 va = normalize (vec3(size.xy, s21 - s01));
                                                                     vec3 vb = normalize (vec3(size.yx, s12 - s10));
                                                                     vec4 normal = vec4(cross(va, vb), depth.z);
                                                                     vec4 bump = texture2D(bumpMap, position);
                                                                     vec4 specularInfos = texture2D(specularTexture, position);
                                                                     vec3 sLightPos = vec3 (lightPos.x, lightPos.y, -lightPos.z * (gl_DepthRange.far - gl_DepthRange.near));
                                                                     float radius = lightPos.w;
                                                                     vec3 pixPos = vec3 (gl_FragCoord.x, gl_FragCoord.y, -normal.w * (gl_DepthRange.far - gl_DepthRange.near));
                                                                     vec4 lightMapColor = texture2D(lightMap, position);
                                                                     vec3 viewPos = vec3(resolution.x * 0.5f, resolution.y * 0.5f, 0);
                                                                     float z = gl_FragCoord.z;
                                                                     vec3 vertexToLight = sLightPos - pixPos;
                                                                     if (bump.x != 0 || bump.y != 0 || bump.z != 0) {
                                                                         float s01 = textureOffset(depthTexture, position, off.xy).z;
                                                                         float s21 = textureOffset(depthTexture, position, off.zy).z;
                                                                         float s10 = textureOffset(depthTexture, position, off.yx).z;
                                                                         float s12 = textureOffset(depthTexture, position, off.yz).z;
                                                                         vec3 tmpNormal = (normal.xyz);
                                                                         vec3 tangeant = normalize (vec3(size.xy, s21 - s01));
                                                                         vec3 binomial = normalize (vec3(size.yx, s12 - s10));
                                                                         normal.x = dot(vertexToLight, tangeant);
                                                                         normal.y = dot(vertexToLight, binomial);
                                                                         normal.z = dot(vertexToLight, tmpNormal);
                                                                         normal.w = bump.w;
                                                                     }
                                                                     if (z >= normal.w) {
                                                                         vec4 specularColor = vec4(0, 0, 0, 0);
                                                                         float attenuation = 1.f - length(vertexToLight) / radius;
                                                                         vec3 pixToView = pixPos - viewPos;
                                                                         float normalLength = dot(normal.xyz, vertexToLight);
                                                                         vec3 lightReflect = vertexToLight + 2 * (normal.xyz * normalLength - vertexToLight);
                                                                         float m = specularInfos.r;
                                                                         float p = specularInfos.g;
                                                                         float specularFactor = dot(normalize(pixToView), normalize(lightReflect));
                                                                         specularFactor = pow (specularFactor, p);
                                                                         if (specularFactor > 0) {
                                                                             specularColor = vec4(lightColor.rgb, 1) * m * specularFactor;
                                                                         }
                                                                         if (normal.x != 0 || normal.y != 0 || normal.z != 0 && vertexToLight.z > 0.f) {
                                                                             vec3 dirToLight = normalize(vertexToLight.xyz);
                                                                             float nDotl = dot (dirToLight, normal.xyz);
                                                                             attenuation *= nDotl;

                                                                         }
                                                                         fColor = vec4(lightColor.xyz, 1) * max(0.0f, attenuation) + specularColor;
                                                                     } else {
                                                                         fColor = lightMapColor;
                                                                     }
                                                                 }
                                                                 )";
                        if (!depthBufferGenerator.loadFromMemory(vertexShader, depthGenFragShader))
                            throw core::Erreur(50, "Failed to load depth buffer generator shader", 0);
                        std::cout<<"depth buffer generator compiled"<<std::endl;
                        if (!normalMapGenerator.loadFromMemory(buildNormalMapVertexShader, buildNormalMapFragmentShader))
                            throw core::Erreur(51, "Failed to load normal generator shader", 0);
                        std::cout<<"normal map generator compiled"<<std::endl;
                        if (!specularTextureGenerator.loadFromMemory(vertexShader, specularGenFragShader))
                            throw core::Erreur(52, "Failed to load specular texture generator shader", 0);
                        std::cout<<"specular texture generator compiled"<<std::endl;
                        if (!bumpTextureGenerator.loadFromMemory(vertexShader, bumpGenFragShader))
                            throw core::Erreur(53, "Failed to load bump texture generator shader", 0);
                        std::cout<<"bump texture generator compiled"<<std::endl;
                        if (!lightMapGenerator.loadFromMemory(buildNormalMapVertexShader, perPixLightingFragmentShader))
                            throw core::Erreur(54, "Failed to load light map generator shader", 0);
                        normalMapGenerator.setParameter("texture", Shader::CurrentTexture);
                        depthBufferGenerator.setParameter("texture", Shader::CurrentTexture);
                        specularTextureGenerator.setParameter("texture",Shader::CurrentTexture);
                        specularTextureGenerator.setParameter("maxM", Material::getMaxSpecularIntensity());
                        specularTextureGenerator.setParameter("maxP", Material::getMaxSpecularPower());
                        bumpTextureGenerator.setParameter("texture",Shader::CurrentTexture);
                        lightMapGenerator.setParameter("resolution", resolution.x, resolution.y, resolution.z);
                        lightMapGenerator.setParameter("depthTexture", depthBuffer.getTexture());
                        lightMapGenerator.setParameter("specularTexture",specularTexture.getTexture());
                        lightMapGenerator.setParameter("bumpMap",bumpTexture.getTexture());
                        lightMapGenerator.setParameter("lightMap",lightMap.getTexture());
                    } else {
                        if (Shader::isAvailable()) {
                            const std::string  vertexShader =
                            "#version 130 \n"
                            "out mat4 projMat;"
                            "void main () {"
                                "gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
                                "gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;"
                                "gl_FrontColor = gl_Color;"
                                "projMat = gl_ProjectionMatrix;"
                            "}";
                            const std::string  buildNormalMapVertexShader =
                            "#version 130 \n"
                            "void main () {"
                                "gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
                                "gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;"
                                "gl_FrontColor = gl_Color;"
                            "}";
                            const std::string buildNormalMapFragmentShader =
                            "#version 130 \n"
                            "const vec2 size = vec2(2.0,0.0);"
                            "const ivec3 off = ivec3(-1,0,1);"
                            "uniform sampler2D texture;"
                            "void main() {"
                                "vec4 depth = texture2D(texture, gl_TexCoord[0].xy);"
                                "float s01 = textureOffset(texture, gl_TexCoord[0].xy, off.xy).z;"
                                "float s21 = textureOffset(texture, gl_TexCoord[0].xy, off.zy).z;"
                                "float s10 = textureOffset(texture, gl_TexCoord[0].xy, off.yx).z;"
                                "float s12 = textureOffset(texture, gl_TexCoord[0].xy, off.yz).z;"
                                "vec3 va = normalize (vec3(size.xy, s21 - s01));"
                                "vec3 vb = normalize (vec3(size.yx, s12 - s10));"
                                "gl_FragColor = vec4(cross(va, vb), depth.z);"
                            "}";
                            const std::string depthGenFragShader =
                            "#version 130 \n"
                            "uniform sampler2D texture;"
                            "uniform float haveTexture;"
                            "in mat4 projMat;"
                            "mat4 inverse(mat4 m) {"
                            "     float"
                            "     a00 = m[0][0], a01 = m[0][1], a02 = m[0][2], a03 = m[0][3],"
                            "     a10 = m[1][0], a11 = m[1][1], a12 = m[1][2], a13 = m[1][3],"
                            "     a20 = m[2][0], a21 = m[2][1], a22 = m[2][2], a23 = m[2][3],"
                            "     a30 = m[3][0], a31 = m[3][1], a32 = m[3][2], a33 = m[3][3],"
                            "     b00 = a00 * a11 - a01 * a10,"
                            "     b01 = a00 * a12 - a02 * a10,"
                            "     b02 = a00 * a13 - a03 * a10,"
                            "     b03 = a01 * a12 - a02 * a11,"
                            "     b04 = a01 * a13 - a03 * a11,"
                            "     b05 = a02 * a13 - a03 * a12,"
                            "     b06 = a20 * a31 - a21 * a30,"
                            "     b07 = a20 * a32 - a22 * a30,"
                            "     b08 = a20 * a33 - a23 * a30,"
                            "     b09 = a21 * a32 - a22 * a31,"
                            "     b10 = a21 * a33 - a23 * a31,"
                            "     b11 = a22 * a33 - a23 * a32,"
                            "     det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;"
                            "     return mat4("
                            "                 a11 * b11 - a12 * b10 + a13 * b09,"
                            "                 a02 * b10 - a01 * b11 - a03 * b09,"
                            "                 a31 * b05 - a32 * b04 + a33 * b03,"
                            "                 a22 * b04 - a21 * b05 - a23 * b03,"
                            "                 a12 * b08 - a10 * b11 - a13 * b07,"
                            "                 a00 * b11 - a02 * b08 + a03 * b07,"
                            "                 a32 * b02 - a30 * b05 - a33 * b01,"
                            "                 a20 * b05 - a22 * b02 + a23 * b01,"
                            "                 a10 * b10 - a11 * b08 + a13 * b06,"
                            "                 a01 * b08 - a00 * b10 - a03 * b06,"
                            "                 a30 * b04 - a31 * b02 + a33 * b00,"
                            "                 a21 * b02 - a20 * b04 - a23 * b00,"
                            "                 a11 * b07 - a10 * b09 - a12 * b06,"
                            "                 a00 * b09 - a01 * b07 + a02 * b06,"
                            "                 a31 * b01 - a30 * b03 - a32 * b00,"
                            "                 a20 * b03 - a21 * b01 + a22 * b00) / det;"
                        "}"
                            "void main () {"
                                "vec4 texel = texture2D(texture, gl_TexCoord[0].xy);"
                                "vec4 colors[2];"
                                "colors[1] = texel * gl_Color;"
                                "colors[0] = gl_Color;"
                                "bool b = (haveTexture > 0.9);"
                                "float current_alpha = colors[int(b)].a;"
                                "float z = (gl_FragCoord.w != 1.f) ? (inverse(projMat) * vec4(0, 0, 0, gl_FragCoord.w)).w : gl_FragCoord.z;"
                                "gl_FragColor = vec4(0, 0, z, current_alpha);"
                            "}";
                            const std::string specularGenFragShader =
                            "#version 130 \n"
                            "uniform sampler2D texture;"
                            "uniform float maxM;"
                            "uniform float maxP;"
                            "uniform float m;"
                            "uniform float p;"
                            "uniform float haveTexture;"
                            "in mat4 projMat;"
                            "mat4 inverse(mat4 m) {"
                                "     float"
                                "     a00 = m[0][0], a01 = m[0][1], a02 = m[0][2], a03 = m[0][3],"
                                "     a10 = m[1][0], a11 = m[1][1], a12 = m[1][2], a13 = m[1][3],"
                                "     a20 = m[2][0], a21 = m[2][1], a22 = m[2][2], a23 = m[2][3],"
                                "     a30 = m[3][0], a31 = m[3][1], a32 = m[3][2], a33 = m[3][3],"
                                "     b00 = a00 * a11 - a01 * a10,"
                                "     b01 = a00 * a12 - a02 * a10,"
                                "     b02 = a00 * a13 - a03 * a10,"
                                "     b03 = a01 * a12 - a02 * a11,"
                                "     b04 = a01 * a13 - a03 * a11,"
                                "     b05 = a02 * a13 - a03 * a12,"
                                "     b06 = a20 * a31 - a21 * a30,"
                                "     b07 = a20 * a32 - a22 * a30,"
                                "     b08 = a20 * a33 - a23 * a30,"
                                "     b09 = a21 * a32 - a22 * a31,"
                                "     b10 = a21 * a33 - a23 * a31,"
                                "     b11 = a22 * a33 - a23 * a32,"
                                "     det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;"
                                "     return mat4("
                                "                 a11 * b11 - a12 * b10 + a13 * b09,"
                                "                 a02 * b10 - a01 * b11 - a03 * b09,"
                                "                 a31 * b05 - a32 * b04 + a33 * b03,"
                                "                 a22 * b04 - a21 * b05 - a23 * b03,"
                                "                 a12 * b08 - a10 * b11 - a13 * b07,"
                                "                 a00 * b11 - a02 * b08 + a03 * b07,"
                                "                 a32 * b02 - a30 * b05 - a33 * b01,"
                                "                 a20 * b05 - a22 * b02 + a23 * b01,"
                                "                 a10 * b10 - a11 * b08 + a13 * b06,"
                                "                 a01 * b08 - a00 * b10 - a03 * b06,"
                                "                 a30 * b04 - a31 * b02 + a33 * b00,"
                                "                 a21 * b02 - a20 * b04 - a23 * b00,"
                                "                 a11 * b07 - a10 * b09 - a12 * b06,"
                                "                 a00 * b09 - a01 * b07 + a02 * b06,"
                                "                 a31 * b01 - a30 * b03 - a32 * b00,"
                                "                 a20 * b03 - a21 * b01 + a22 * b00) / det;"
                            "}"
                            "void main() {"
                                "vec4 texel = texture2D(texture, gl_TexCoord[0].xy);"
                                "vec4 colors[2];"
                                "colors[1] = texel * gl_Color;"
                                "colors[0] = gl_Color;"
                                "bool b = (haveTexture > 0.9);"
                                "vec4 color = colors[int(b)];"
                                "float z = (gl_FragCoord.w != 1.f) ? (inverse(projMat) * vec4(0, 0, 0, gl_FragCoord.w)).w : gl_FragCoord.z;"
                                "float intensity = (maxM != 0.f) ? m / maxM : 0.f;"
                                "float power = (maxP != 0.f) ? p / maxP : 0.f;"
                                "gl_FragColor = vec4(intensity, power, z, color.a);"
                            "}";
                             const std::string bumpGenFragShader =
                             "#version 130 \n"
                             "uniform sampler2D texture;"
                             "uniform float haveTexture;"
                             "void main() {"
                                 "vec4 texel = texture2D(texture, gl_TexCoord[0].xy);"
                                 "vec4 colors[2];"
                                 "colors[1] = texel * gl_Color;"
                                 "colors[0] = gl_Color;"
                                 "bool b = (haveTexture > 0.9);"
                                 "vec4 color = colors[int(b)];"
                                 "gl_FragColor = color;"
                             "}";
                             const std::string perPixLightingFragmentShader =
                             "#version 130 \n"
                             "const vec2 size = vec2(2.0,0.0);"
                             "const ivec3 off = ivec3(-1,0,1);"
                             "uniform sampler2D normalMap;"
                             "uniform sampler2D lightMap;"
                             "uniform sampler2D specularTexture;"
                             "uniform sampler2D bumpMap;"
                             "uniform vec3 resolution;"
                             "uniform vec4 lightColor;"
                             "uniform vec4 lightPos;"
                             "in mat4 projMat;"
                             "mat4 inverse(mat4 m) {"
                            "     float"
                            "     a00 = m[0][0], a01 = m[0][1], a02 = m[0][2], a03 = m[0][3],"
                            "     a10 = m[1][0], a11 = m[1][1], a12 = m[1][2], a13 = m[1][3],"
                            "     a20 = m[2][0], a21 = m[2][1], a22 = m[2][2], a23 = m[2][3],"
                            "     a30 = m[3][0], a31 = m[3][1], a32 = m[3][2], a33 = m[3][3],"
                            "     b00 = a00 * a11 - a01 * a10,"
                            "     b01 = a00 * a12 - a02 * a10,"
                            "     b02 = a00 * a13 - a03 * a10,"
                            "     b03 = a01 * a12 - a02 * a11,"
                            "     b04 = a01 * a13 - a03 * a11,"
                            "     b05 = a02 * a13 - a03 * a12,"
                            "     b06 = a20 * a31 - a21 * a30,"
                            "     b07 = a20 * a32 - a22 * a30,"
                            "     b08 = a20 * a33 - a23 * a30,"
                            "     b09 = a21 * a32 - a22 * a31,"
                            "     b10 = a21 * a33 - a23 * a31,"
                            "     b11 = a22 * a33 - a23 * a32,"
                            "     det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;"
                            "     return mat4("
                            "                 a11 * b11 - a12 * b10 + a13 * b09,"
                            "                 a02 * b10 - a01 * b11 - a03 * b09,"
                            "                 a31 * b05 - a32 * b04 + a33 * b03,"
                            "                 a22 * b04 - a21 * b05 - a23 * b03,"
                            "                 a12 * b08 - a10 * b11 - a13 * b07,"
                            "                 a00 * b11 - a02 * b08 + a03 * b07,"
                            "                 a32 * b02 - a30 * b05 - a33 * b01,"
                            "                 a20 * b05 - a22 * b02 + a23 * b01,"
                            "                 a10 * b10 - a11 * b08 + a13 * b06,"
                            "                 a01 * b08 - a00 * b10 - a03 * b06,"
                            "                 a30 * b04 - a31 * b02 + a33 * b00,"
                            "                 a21 * b02 - a20 * b04 - a23 * b00,"
                            "                 a11 * b07 - a10 * b09 - a12 * b06,"
                            "                 a00 * b09 - a01 * b07 + a02 * b06,"
                            "                 a31 * b01 - a30 * b03 - a32 * b00,"
                            "                 a20 * b03 - a21 * b01 + a22 * b00) / det;"
                        "}"
                             "void main () { "
                                 "vec2 position = (gl_FragCoord.xy / resolution.xy);"
                                 "vec4 normal = texture2D(normalMap, position);"
                                 "vec4 bump = texture2D(bumpMap, position);"
                                 "vec4 specularInfos = texture2D(specularTexture, position);"
                                 "vec3 sLightPos = vec3 (lightPos.x, lightPos.y, -lightPos.z * (gl_DepthRange.far - gl_DepthRange.near));"
                                 "float radius = lightPos.w;"
                                 "vec3 pixPos = vec3 (gl_FragCoord.x, gl_FragCoord.y, -normal.w * (gl_DepthRange.far - gl_DepthRange.near));"
                                 "vec4 lightMapColor = texture2D(lightMap, position);"
                                 "vec3 viewPos = vec3(resolution.x * 0.5f, resolution.y * 0.5f, 0);"
                                 "float z = (gl_FragCoord.w != 1.f) ? (inverse(projMat) * vec4(0, 0, 0, gl_FragCoord.w)).w : gl_FragCoord.z;"
                                 "vec3 vertexToLight = sLightPos - pixPos;"
                                 "if (bump.x != 0 || bump.y != 0 || bump.z != 0) {"
                                     "float s01 = textureOffset(normalMap, position, off.xy).z;"
                                     "float s21 = textureOffset(normalMap, position, off.zy).z;"
                                     "float s10 = textureOffset(normalMap, position, off.yx).z;"
                                     "float s12 = textureOffset(normalMap, position, off.yz).z;"
                                     "vec3 tmpNormal = (normal.xyz);"
                                     "vec3 tangeant = normalize (vec3(size.xy, s21 - s01));"
                                     "vec3 binomial = normalize (vec3(size.yx, s12 - s10));"
                                     "normal.x = dot(vertexToLight, tangeant);"
                                     "normal.y = dot(vertexToLight, binomial);"
                                     "normal.z = dot(vertexToLight, tmpNormal);"
                                     "normal.w = bump.w;"
                                 "}"
                                 "if (z >= normal.w) {"
                                     "vec4 specularColor = vec4(0, 0, 0, 0);"
                                     "float attenuation = 1.f - length(vertexToLight) / radius;"
                                     "vec3 pixToView = pixPos - viewPos;"
                                     "float normalLength = dot(normal.xyz, vertexToLight);"
                                     "vec3 lightReflect = vertexToLight + 2 * (normal.xyz * normalLength - vertexToLight);"
                                     "float m = specularInfos.r;"
                                     "float p = specularInfos.g;"
                                     "float specularFactor = dot(normalize(pixToView), normalize(lightReflect));"
                                     "specularFactor = pow (specularFactor, p);"
                                     "if (specularFactor > 0) {"
                                         "specularColor = vec4(lightColor.rgb, 1) * m * specularFactor;"
                                     "}"
                                     "if (normal.x != 0 || normal.y != 0 || normal.z != 0 && vertexToLight.z > 0.f) {"
                                         "vec3 dirToLight = normalize(vertexToLight.xyz);"
                                         "float nDotl = dot (dirToLight, normal.xyz);"
                                         "attenuation *= nDotl;"
                                     "}"
                                     "gl_FragColor = vec4(lightColor.xyz, 1) * max(0.0f, attenuation) + specularColor;"
                                 "} else {"
                                     "gl_FragColor = lightMapColor;"
                                 "}"
                            "}";
                            if (!depthBufferGenerator.loadFromMemory(vertexShader, depthGenFragShader))
                                throw core::Erreur(50, "Failed to load depth buffer generator shader", 0);
                            if (!normalMapGenerator.loadFromMemory(buildNormalMapVertexShader, buildNormalMapFragmentShader))
                                throw core::Erreur(51, "Failed to load normal generator shader", 0);
                            if (!specularTextureGenerator.loadFromMemory(vertexShader, specularGenFragShader))
                                throw core::Erreur(52, "Failed to load specular texture generator shader", 0);
                            if (!bumpTextureGenerator.loadFromMemory(buildNormalMapVertexShader, bumpGenFragShader))
                                throw core::Erreur(53, "Failed to load bump texture generator shader", 0);
                            if (!lightMapGenerator.loadFromMemory(vertexShader, perPixLightingFragmentShader))
                                throw core::Erreur(54, "Failed to load light map generator shader", 0);
                            normalMapGenerator.setParameter("texture", Shader::CurrentTexture);
                            depthBufferGenerator.setParameter("texture", Shader::CurrentTexture);
                            specularTextureGenerator.setParameter("texture",Shader::CurrentTexture);
                            specularTextureGenerator.setParameter("maxM", Material::getMaxSpecularIntensity());
                            specularTextureGenerator.setParameter("maxP", Material::getMaxSpecularPower());
                            bumpTextureGenerator.setParameter("texture",Shader::CurrentTexture);
                            lightMapGenerator.setParameter("resolution", resolution.x, resolution.y, resolution.z);
                            lightMapGenerator.setParameter("normalMap", normalMap.getTexture());
                            lightMapGenerator.setParameter("specularTexture",specularTexture.getTexture());
                            lightMapGenerator.setParameter("bumpMap",bumpTexture.getTexture());
                            lightMapGenerator.setParameter("lightMap",lightMap.getTexture());
                        } else {
                            throw core::Erreur(55, "Shader not supported!", 0);
                        }
                    }
            }
            void LightRenderComponent::pushEvent(window::IEvent event, RenderWindow& rw) {
                if (event.type == window::IEvent::WINDOW_EVENT && event.window.type == window::IEvent::WINDOW_EVENT_RESIZED && &getWindow() == &rw && isAutoResized()) {
                    std::cout<<"recompute size"<<std::endl;
                    recomputeSize();
                    getListener().pushEvent(event);
                    getView().reset(physic::BoundingBox(getView().getViewport().getPosition().x, getView().getViewport().getPosition().y, getView().getViewport().getPosition().z, event.window.data1, event.window.data2, getView().getViewport().getDepth()));
                }
            }
            bool LightRenderComponent::needToUpdate() {
            return update;
        }
        void LightRenderComponent::changeVisibleEntities(Entity* toRemove, Entity* toAdd, EntityManager* em) {
            bool removed;
            em->removeAnimatedVisibleEntity(toRemove, visibleEntities, view, removed);
            if (removed) {
                em->insertAnimatedVisibleEntity(toAdd, visibleEntities, view);
                loadEntitiesOnComponent(visibleEntities);
                update = true;
            }
        }
        std::string LightRenderComponent::getExpression() {
            return expression;
        }
        void LightRenderComponent::clear() {
             depthBuffer.clear(sf::Color::Transparent);
             normalMap.clear(sf::Color::Transparent);
             specularTexture.clear(sf::Color::Transparent);
             bumpTexture.clear(sf::Color::Transparent);
             sf::Color ambientColor = g2d::AmbientLight::getAmbientLight().getColor();
             lightMap.clear(ambientColor);
        }
        Sprite& LightRenderComponent::getNormalMapTile () {
            return normalMapTile;
        }
        Sprite& LightRenderComponent::getDepthBufferTile() {
            return depthBufferTile;
        }
        Sprite& LightRenderComponent::getspecularTile () {
            return specularBufferTile;
        }
        Sprite& LightRenderComponent::getBumpTile() {
            return bumpMapTile;
        }
        Sprite& LightRenderComponent::getLightTile() {
            return lightMapTile;
        }
        const Texture& LightRenderComponent::getDepthBufferTexture() {
            return depthBuffer.getTexture();
        }
        const Texture& LightRenderComponent::getnormalMapTexture() {
            return normalMap.getTexture();
        }
        const Texture& LightRenderComponent::getSpecularTexture() {
            return specularTexture.getTexture();
        }
        const Texture& LightRenderComponent::getbumpTexture() {
            return bumpTexture.getTexture();
        }
        const Texture& LightRenderComponent::getLightMapTexture() {
            return lightMap.getTexture();
        }
        bool LightRenderComponent::loadEntitiesOnComponent(std::vector<Entity*> vEntities)
        {
            batcher.clear();
            lightBatcher.clear();
            for (unsigned int i = 0; i < vEntities.size(); i++) {

                //if ( vEntities[i]->isLeaf()) {

                    if (vEntities[i]->isLight()) {
                        for (unsigned int j = 0; j <  vEntities[i]->getNbFaces(); j++) {
                            lightBatcher.addFace(vEntities[i]->getFace(j));
                        }
                    } else {
                        for (unsigned int j = 0; j <  vEntities[i]->getNbFaces(); j++) {
                            batcher.addFace(vEntities[i]->getFace(j));
                        }
                    }
                //}
            }
            m_instances = batcher.getInstances();
            m_light_instances = lightBatcher.getInstances();
            visibleEntities = vEntities;
            update = true;
            return true;
        }
        void LightRenderComponent::setView(View view){
            this->view = view;
            depthBuffer.setView(view);
            normalMap.setView(view);
            specularTexture.setView(view);
            bumpTexture.setView(view);
            lightMap.setView(view);
        }
        void LightRenderComponent::setExpression(std::string expression) {
            update = true;
            this->expression = expression;
        }
        void LightRenderComponent::drawNextFrame() {
            update = false;
            RenderStates states;
            states.blendMode = sf::BlendNone;
            physic::BoundingBox viewArea = view.getViewVolume();
            math::Vec3f position (viewArea.getPosition().x,viewArea.getPosition().y, view.getPosition().z);
            math::Vec3f size (viewArea.getWidth(), viewArea.getHeight(), 0);
            if (lightMap.getSettings().versionMajor >= 3 && lightMap.getSettings().versionMinor >= 3) {
                    math::Matrix4f viewMatrix = view.getViewMatrix().getMatrix().transpose();
                    math::Matrix4f projMatrix = view.getProjMatrix().getMatrix().transpose();
                    depthBufferGenerator.setParameter("projectionMatrix", projMatrix);
                    depthBufferGenerator.setParameter("viewMatrix", viewMatrix);
                    specularTextureGenerator.setParameter("projectionMatrix", projMatrix);
                    specularTextureGenerator.setParameter("viewMatrix", viewMatrix);
                    bumpTextureGenerator.setParameter("projectionMatrix", projMatrix);
                    bumpTextureGenerator.setParameter("viewMatrix", viewMatrix);
                    for (unsigned int i = 0; i < m_instances.size(); i++) {

                        if (m_instances[i].getAllVertices().getVertexCount() > 0) {
                            float specularIntensity = m_instances[i].getMaterial().getSpecularIntensity();
                            float specularPower = m_instances[i].getMaterial().getSpecularPower();
                            specularTextureGenerator.setParameter("m", specularIntensity);
                            specularTextureGenerator.setParameter("p", specularPower);
                            if (m_instances[i].getMaterial().getTexture() != nullptr) {
                                math::Matrix4f texMatrix = m_instances[i].getMaterial().getTexture()->getTextureMatrix();
                                depthBufferGenerator.setParameter("textureMatrix", texMatrix);
                                depthBufferGenerator.setParameter("haveTexture", 1.f);
                                specularTextureGenerator.setParameter("textureMatrix", texMatrix);
                                specularTextureGenerator.setParameter("haveTexture", 1.f);
                            } else {
                                depthBufferGenerator.setParameter("haveTexture", 0.f);
                                specularTextureGenerator.setParameter("haveTexture", 0.f);
                            }
                            if (m_instances[i].getMaterial().getBumpTexture() != nullptr) {
                                math::Matrix4f texMatrix = m_instances[i].getMaterial().getBumpTexture()->getTextureMatrix();
                                bumpTextureGenerator.setParameter("textureMatrix", texMatrix);
                                bumpTextureGenerator.setParameter("haveTexture", 1.f);
                            } else {
                                bumpTextureGenerator.setParameter("haveTexture", 0.f);
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
                            states.texture = m_instances[i].getMaterial().getTexture();
                            states.shader = &depthBufferGenerator;
                            depthBuffer.drawInstanced(vb, vboWorldMatrices, m_instances[i].getVertexArrays()[0]->getPrimitiveType(), 0, m_instances[i].getVertexArrays()[0]->getVertexCount(), tm.size(), states);
                            /*states.shader = &specularTextureGenerator;
                            specularTexture.drawInstanced(vb, vboWorldMatrices, m_instances[i].getVertexArrays()[0]->getPrimitiveType(), 0, m_instances[i].getVertexArrays()[0]->getVertexCount(), tm.size(), states);
                            states.shader = &bumpTextureGenerator;
                            states.texture = m_instances[i].getMaterial().getBumpTexture();
                            bumpTexture.drawInstanced(vb, vboWorldMatrices, m_instances[i].getVertexArrays()[0]->getPrimitiveType(), 0, m_instances[i].getVertexArrays()[0]->getVertexCount(), tm.size(), states);*/
                        }
                    }
                    depthBuffer.display();
                    specularTexture.display();
                    bumpTexture.display();
                    /*states.shader = &normalMapGenerator;
                    VertexArray va = depthBufferTile.getVertexArray();
                    depthBufferTile.setCenter(view.getPosition());
                    vb.clear();
                    vb.setPrimitiveType(va.getPrimitiveType());
                    for (unsigned int n = 0; n < va.getVertexCount(); n++) {
                        vb.append(va[n]);
                    }
                    vb.update();
                    math::Matrix4f worldMatrix = depthBufferTile.getTransform().getMatrix().transpose();
                    math::Matrix4f texMatrix = depthBufferTile.getTexture()->getTextureMatrix();
                    normalMapGenerator.setParameter("projectionMatrix", projMatrix);
                    normalMapGenerator.setParameter("viewMatrix", viewMatrix);
                    normalMapGenerator.setParameter("textureMatrix", texMatrix);
                    normalMapGenerator.setParameter("worldMatrix", worldMatrix);
                    states.texture = depthBufferTile.getTexture();
                    normalMap.drawVertexBuffer(vb, states);
                    normalMap.display();*/
                    states.shader = &lightMapGenerator;
                    states.blendMode = sf::BlendAdd;
                    lightMapGenerator.setParameter("projectionMatrix", projMatrix);
                    lightMapGenerator.setParameter("viewMatrix", viewMatrix);
                    for (unsigned int i = 0; i < m_light_instances.size(); i++) {
                        if (m_light_instances[i].getAllVertices().getVertexCount() > 0) {
                            for (unsigned int j = 0; j < m_light_instances[i].getVertexArrays().size(); j++) {
                                vb.clear();
                                vb.setPrimitiveType( m_light_instances[i].getVertexArrays()[j]->getPrimitiveType());
                                for (unsigned int n = 0; n < m_light_instances[i].getVertexArrays()[j]->getVertexCount(); n++) {
                                    vb.append((*m_light_instances[i].getVertexArrays()[j])[n]);
                                }
                                vb.update();
                                math::Matrix4f m = m_light_instances[i].getTransforms()[j]->getMatrix().transpose();
                                lightMapGenerator.setParameter("worldMatrix", m);
                                EntityLight* el = static_cast<EntityLight*> (m_light_instances[i].getVertexArrays()[j]->getEntity());
                                math::Vec3f center = getWindow().mapCoordsToPixel(el->getLightCenter(), view);
                                center.w = el->getSize().x * 0.5f;
                                lightMapGenerator.setParameter("lightPos", center.x, center.y, center.z, center.w);
                                lightMapGenerator.setParameter("lightColor", el->getColor().r, el->getColor().g,el->getColor().b,el->getColor().a);
                                lightMap.drawVertexBuffer(vb, states);
                            }
                        }
                    }
                    lightMap.display();
            } else {
                for (unsigned int i = 0; i < m_instances.size(); i++) {
                    if (m_instances[i].getAllVertices().getVertexCount() > 0) {
                        float specularIntensity = m_instances[i].getMaterial().getSpecularIntensity();
                        float specularPower = m_instances[i].getMaterial().getSpecularPower();
                        specularTextureGenerator.setParameter("m", specularIntensity);
                        specularTextureGenerator.setParameter("p", specularPower);
                        if (m_instances[i].getMaterial().getTexture() != nullptr) {
                            depthBufferGenerator.setParameter("haveTexture", 1.f);
                            specularTextureGenerator.setParameter("haveTexture", 1.f);
                        } else {
                            depthBufferGenerator.setParameter("haveTexture", 0.f);
                            specularTextureGenerator.setParameter("haveTexture", 0.f);
                        }
                        if (m_instances[i].getMaterial().getBumpTexture() != nullptr) {
                            bumpTextureGenerator.setParameter("haveTexture", 1.f);
                        } else {
                            bumpTextureGenerator.setParameter("haveTexture", 0.f);
                        }
                        states.texture = m_instances[i].getMaterial().getTexture();
                        states.shader = &depthBufferGenerator;
                        depthBuffer.draw(m_instances[i].getAllVertices(), states);
                        /*states.shader = &specularTextureGenerator;
                        specularTexture.draw(m_instances[i].getAllVertices(), states);
                        states.shader = &bumpTextureGenerator;
                        states.texture = m_instances[i].getMaterial().getBumpTexture();
                        bumpTexture.draw(m_instances[i].getAllVertices(), states);*/
                    }
                }
                states.shader = &normalMapGenerator;
                depthBufferTile.setCenter(view.getPosition());
                depthBuffer.display();
                normalMap.draw(depthBufferTile, states);
                states.shader = &lightMapGenerator;
                states.blendMode = sf::BlendAdd;
                for (unsigned int i = 0; i < m_light_instances.size(); i++) {
                    if (m_light_instances[i].getAllVertices().getVertexCount() > 0) {
                        for (unsigned int j = 0; j < m_light_instances[i].getVertexArrays().size(); j++) {
                            //std::cout<<"draw light"<<std::endl;
                            states.transform =  *m_light_instances[i].getTransforms()[j];
                            EntityLight* el = static_cast<EntityLight*> (m_light_instances[i].getVertexArrays()[j]->getEntity());
                            math::Vec3f center = getWindow().mapCoordsToPixel(el->getLightCenter(), view);
                            center.w = el->getSize().x * 0.5f;
                            lightMapGenerator.setParameter("lightPos", center.x, center.y, center.z, center.w);
                            lightMapGenerator.setParameter("lightColor", el->getColor().r, el->getColor().g,el->getColor().b,el->getColor().a);
                            lightMap.draw(*m_light_instances[i].getVertexArrays()[j], states);
                        }
                    }
                }
                specularTexture.display();
                bumpTexture.display();
                normalMap.display();
                lightMap.display();
            }
        }
        std::vector<Entity*> LightRenderComponent::getEntities() {
            return visibleEntities;
        }
        void LightRenderComponent::draw(RenderTarget& target, RenderStates states) {
            lightMapTile.setCenter(target.getView().getPosition());
            states.blendMode = sf::BlendMultiply;
            target.draw(lightMapTile, states);
        }
        View& LightRenderComponent::getView() {
            return view;
        }
        int LightRenderComponent::getLayer() {
            return getPosition().z;
        }
        void LightRenderComponent::updateParticleSystems() {
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
