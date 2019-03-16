#include "../../../include/odfaeg/Graphics/oitRenderComponent.h"
#include "glCheck.h"
#include <memory.h>
using namespace sf;
using namespace std;
namespace odfaeg {
    namespace graphic {
        OITRenderComponent::OITRenderComponent (RenderWindow& window, int layer, std::string expression, window::ContextSettings settings) :
            HeavyComponent(window, math::Vec3f(window.getView().getPosition().x, window.getView().getPosition().y, layer),
                          math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0),
                          math::Vec3f(window.getView().getSize().x + window.getView().getSize().x * 0.5f, window.getView().getPosition().y + window.getView().getSize().y * 0.5f, layer)),
            view(window.getView()),
            expression(expression) {
            update = false;
            sf::Vector3i resolution ((int) window.getSize().x, (int) window.getSize().y, window.getView().getSize().z);
            settings.depthBits = 32;
            depthBuffer.create(resolution.x, resolution.y,settings);
            frontBuffer.create(resolution.x, resolution.y,settings);
            opaquePixels.create(resolution.x, resolution.y, settings);
            settings.depthBits = 0;
            semiTransparentPixels.create(resolution.x, resolution.y, settings);
            weightedBlendedOITPass1.create(resolution.x, resolution.y, settings);
            weightedBlendedOITPass2.create(resolution.x, resolution.y, settings);
            frameBuffer.create(resolution.x, resolution.y,settings);
            bumpTexture.create(resolution.x, resolution.y,settings);
            refractionTexture.create(resolution.x, resolution.y,settings);
            frontBuffer.setView(window.getView());
            depthBuffer.setView(window.getView());
            frameBuffer.setView(window.getView());
            bumpTexture.setView(window.getView());
            weightedBlendedOITPass1.setView(window.getView());
            weightedBlendedOITPass2.setView(window.getView());
            refractionTexture.setView(window.getView());
            semiTransparentPixels.setView(window.getView());
            opaquePixels.setView(window.getView());
            frontBufferTile = Sprite(frontBuffer.getTexture(), math::Vec3f(0, 0, 0), math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0), IntRect(0, 0, window.getView().getSize().x, window.getView().getSize().y));
            depthBufferTile = Sprite(depthBuffer.getTexture(), math::Vec3f(0, 0, 0), math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0), IntRect(0, 0, window.getView().getSize().x, window.getView().getSize().y));
            frameBufferTile = Sprite(frameBuffer.getTexture(), math::Vec3f(0, 0, 0), math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0), IntRect(0, 0, window.getView().getSize().x, window.getView().getSize().y));
            opaquePixelsTile = Sprite(opaquePixels.getTexture(), math::Vec3f(0, 0, 0), math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0), IntRect(0, 0, window.getView().getSize().x, window.getView().getSize().y));
            semiTransparentsPixelsTile = Sprite(semiTransparentPixels.getTexture(), math::Vec3f(0, 0, 0), math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0), IntRect(0, 0, window.getView().getSize().x, window.getView().getSize().y));
            weightedBlendedOITPass1Tile = Sprite(weightedBlendedOITPass1.getTexture(), math::Vec3f(0, 0, 0), math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0), IntRect(0, 0, window.getView().getSize().x, window.getView().getSize().y));
            weightedBlendedOITPass2Tile = Sprite(weightedBlendedOITPass2.getTexture(), math::Vec3f(0, 0, 0), math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, 0), IntRect(0, 0, window.getView().getSize().x, window.getView().getSize().y));
            core::FastDelegate<bool> signal (&OITRenderComponent::needToUpdate, this);
            core::FastDelegate<void> slot (&OITRenderComponent::drawNextFrame, this);
            core::Command cmd(signal, slot);
            getListener().connect("UPDATE", cmd);
            if (Shader::isAvailable()) {
                const std::string  simpleVertexShader =
                "#version 130 \n"
                "void main () {"
                    "gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
                    "gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;"
                    "gl_FrontColor = gl_Color;"
                "}";
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
                "in mat4 projMat;"
                "uniform sampler2D texture;"
                "uniform float haveTexture;"
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
                    "vec4 color = colors[int(b)];"
                    "float z = /*(gl_FragCoord.w != 1.f) ? (inverse(projMat) * vec4(0, 0, 0, gl_FragCoord.w)).w : */gl_FragCoord.z;"
                    "colors[0] = vec4(0, 0, 0, 0);"
                    "colors[1] = vec4(0, 0, z, color.a);"
                    "b = (color.a == 1);"
                    "gl_FragColor = colors[int(b)]; /*vec4(0, 0, z, color.a);*/"
                "}";
                const std::string frameBufferGenFragShader =
                "#version 130 \n"
                "uniform sampler2D depthBuffer;"
                "uniform sampler2D texture;"
                "uniform vec3 resolution;"
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
                    "vec2 position = ( gl_FragCoord.xy / resolution.xy );"
                    "float max_z = texture2D(depthBuffer, position).z;"
                    "vec4 texel = texture2D(texture, gl_TexCoord[0].xy);"
                    "vec4 colors[2];"
                    "colors[1] = texel * gl_Color;"
                    "colors[0] = gl_Color;"
                    "bool b = (haveTexture > 0.9);"
                    "vec4 color = colors[int(b)];"
                    "float z = (gl_FragCoord.w != 1.f) ? (inverse(projMat) * vec4(0, 0, 0, gl_FragCoord.w)).w : gl_FragCoord.z;"
                    "colors[1] = color;"
                    "colors[0] = vec4(0, 0, 0, 0);"
                    "b = (z < max_z);"
                    "gl_FragColor = colors[int(b)];"
                "}";
                const std::string bumpGenFragShader =
                "#version 130 \n"
                "uniform sampler2D bumpTexture;"
                "uniform sampler2D texture;"
                "uniform vec3 resolution;"
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
                    "vec2 position = ( gl_FragCoord.xy / resolution.xy );"
                    "vec4 refraction = texture2D(refractionTexture, position);"
                    "float z = (gl_FragCoord.w != 1.f) ? (inverse(projMat) * vec4(0, 0, 0, gl_FragCoord.w)).w : gl_FragCoord.z;"
                    "if (z >= refraction.a) {"
                        "gl_FragColor = vec4(refractionFactor, 0, z, 0);"
                    "} else {"
                        "gl_FragColor = refraction;"
                    "}"
                "}";
                const std::string filterNotOpaquePixelsFragShad =
                "#version 130\n"
                "uniform sampler2D texture;"
                "uniform float haveTexture;"
                "void main() {"
                "   vec4 colors[2];"
                "   colors[0] = gl_Color;"
                "   colors[1] = gl_Color * texture2D(texture, gl_TexCoord[0].xy);"
                "   bool b = (haveTexture > 0.9f);"
                "   vec4 color = colors[int(b)];"
                "   colors[0] = vec4(0, 0, 0, 0);"
                "   colors[1] = color;"
                "   b = (color.a == 1);"
                "   gl_FragColor = colors[int(b)];"
                "}";
                const std::string generateWeightedBlendedOITPass1FragShader =
                   "#version 130 \n"
                   "in mat4 projMat;"
                   "uniform sampler2D texture;"
                   "uniform sampler2D depthBuffer;"
                   "uniform float haveTexture;"
                   "uniform vec3 resolution;"
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
                   "    vec2 position = ( gl_FragCoord.xy / resolution.xy );"
                   "    float max_z = texture2D(depthBuffer, position).z;"
                   "    vec4 color = (haveTexture > 0.9) ? gl_Color * texture2D(texture, gl_TexCoord[0].xy) : gl_Color;"
                   "    float z = (gl_FragCoord.w != 1.f) ? (inverse(projMat) * vec4(0, 0, 0, gl_FragCoord.w)).w : gl_FragCoord.z;"
                   "    float weight = max(min(1.0, max(max(color.r, color.g), color.b) * color.a), color.a) * clamp(0.03 / (1e-5 + pow(z / 200, 4.0)), 1e-2, 3e3);"
                   "    vec4 colors[2];"
                   "    colors[0] = vec4(0, 0, 0, 0);"
                   "    colors[1] = vec4(color.rgb * color.a, color.a) * weight;"
                   "    bool b = (color.a != 1 && z >= max_z);"
                   "    gl_FragColor = colors[int(b)];"
                   "}";
                   const std::string generateWeightedBlendedOITPass2FragShader =
                   "#version 130 \n"
                   "in mat4 projMat;"
                   "uniform sampler2D texture;"
                   "uniform sampler2D depthBuffer;"
                   "uniform float haveTexture;"
                   "uniform vec3 resolution;"
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
                   "    vec2 position = ( gl_FragCoord.xy / resolution.xy );"
                   "    vec4 color = (haveTexture > 0.9) ? gl_Color * texture2D(texture, gl_TexCoord[0].xy) : gl_Color;"
                   "    float max_z = texture2D(depthBuffer, position).z;"
                   "    float z = (gl_FragCoord.w != 1.f) ? (inverse(projMat) * vec4(0, 0, 0, gl_FragCoord.w)).w : gl_FragCoord.z;"
                   "    vec4 colors[2];"
                   "    colors[0] = vec4(0, 0, 0, 0);"
                   "    colors[1] = color;"
                   "    bool b = (color.a != 1 && z >= max_z);"
                   "    gl_FragColor = colors[int(b)];"
                   "}";
                   const std::string filterOpaquePixelsFragShad =
                   "#version 130 \n"
                   "uniform sampler2D rt0;"
                   "uniform sampler2D rt1;"
                   "uniform sampler2D depthBuffer;"
                   "uniform sampler2D texture;"
                   "uniform float haveTexture;"
                   "uniform vec3 resolution;"
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
                   "    vec2 position = ( gl_FragCoord.xy / resolution.xy );"
                   "    vec4 colors[2];"
                   "    colors[0] = gl_Color;"
                   "    colors[1] = gl_Color * texture2D(texture, gl_TexCoord[0].xy);"
                   "    bool b = (haveTexture > 0.9);"
                   "    vec4 color = colors[int(b)];"
                   "    vec4 accum = texture2D(rt0, position);"
                   "    float reveal = texture2D(rt1, position).a;"
                   "    float z = (gl_FragCoord.w != 1.f) ? (inverse(projMat) * vec4(0, 0, 0, gl_FragCoord.w)).w : gl_FragCoord.z;"
                   "    float max_z = texture2D(depthBuffer, position).z;"
                   "    colors[0] = vec4(0, 0, 0, 0);"
                   "    colors[1] = /*color;*/ vec4(accum.rgb / max(accum.a, 1e-5), reveal); /*vec4(vec3(1, 1, 1) * color.a * color.rgb * color.a, 1);*/"
                   "    b = (color.a != 1 && z >= max_z);"
                   "    gl_FragColor = vec4(accum.rgb / max(accum.a, 1e-5), reveal);"
                   "    gl_FragColor = colors[int(b)];"
                   "}";
                if (!depthBufferGenerator.loadFromMemory(vertexShader, depthGenFragShader))
                    throw core::Erreur(50, "Failed to load depth buffer generator shader", 0);
                if (!frameBufferGenerator.loadFromMemory(vertexShader, frameBufferGenFragShader))
                    throw core::Erreur(51, "Failed to load frame buffer generator shader", 0);
                if (!specularTextureGenerator.loadFromMemory(vertexShader, specularGenFragShader))
                    throw core::Erreur(52, "Failed to load specular texture generator shader", 0);
                if (!bumpTextureGenerator.loadFromMemory(vertexShader, bumpGenFragShader))
                    throw core::Erreur(53, "Failed to load bump texture generator shader", 0);
                if (!refractionTextureGenerator.loadFromMemory(vertexShader, refractionGenFragShader))
                    throw core::Erreur(54, "Failed to load refraction texture generator shader", 0);
                if (!filterNotOpaquePixels.loadFromMemory(simpleVertexShader, filterNotOpaquePixelsFragShad)) {
                    throw core::Erreur(55, "Failed to load filter not opaque pixels shader", 0);
                }
                if (!generateWeightedBlendedOITPass1.loadFromMemory(vertexShader, generateWeightedBlendedOITPass1FragShader)) {
                    throw core::Erreur(56, "Failed to load generate weighted blended oit pass 1 shader", 0);
                }
                if (!generateWeightedBlendedOITPass2.loadFromMemory(vertexShader, generateWeightedBlendedOITPass2FragShader)) {
                    throw core::Erreur(56, "Failed to load generate weighted blended oit pass 2 shader", 0);
                }
                if (!filterOpaquePixels.loadFromMemory(vertexShader, filterOpaquePixelsFragShad)) {
                    throw core::Erreur(57, "Failed to load generate filter opaque pixels shader", 0);
                }
                filterNotOpaquePixels.setParameter("texture", Shader::CurrentTexture);
                generateWeightedBlendedOITPass1.setParameter("texture", Shader::CurrentTexture);
                generateWeightedBlendedOITPass1.setParameter("depthBuffer", depthBuffer.getTexture());
                generateWeightedBlendedOITPass1.setParameter("resolution", resolution.x, resolution.y, resolution.z);
                generateWeightedBlendedOITPass2.setParameter("depthBuffer", depthBuffer.getTexture());
                generateWeightedBlendedOITPass2.setParameter("texture", Shader::CurrentTexture);
                generateWeightedBlendedOITPass2.setParameter("resolution", resolution.x, resolution.y, resolution.z);
                filterOpaquePixels.setParameter("texture", Shader::CurrentTexture);
                filterOpaquePixels.setParameter("resolution", resolution.x, resolution.y, resolution.z);
                filterOpaquePixels.setParameter("depthBuffer", depthBuffer.getTexture());
                filterOpaquePixels.setParameter("rt0", weightedBlendedOITPass1.getTexture());
                filterOpaquePixels.setParameter("rt1", weightedBlendedOITPass2.getTexture());
                frameBufferGenerator.setParameter("resolution",resolution.x, resolution.y, resolution.z);
                frameBufferGenerator.setParameter("depthBuffer", depthBuffer.getTexture());
                frameBufferGenerator.setParameter("texture", Shader::CurrentTexture);
                depthBufferGenerator.setParameter("texture", Shader::CurrentTexture);
                specularTextureGenerator.setParameter("resolution",resolution.x, resolution.y, resolution.z);
                //specularTextureGenerator->setParameter("specularTexture",specularTexture->getTexture());
                specularTextureGenerator.setParameter("texture",Shader::CurrentTexture);
                specularTextureGenerator.setParameter("maxM", Material::getMaxSpecularIntensity());
                specularTextureGenerator.setParameter("maxP", Material::getMaxSpecularPower());
                bumpTextureGenerator.setParameter("resolution",resolution.x, resolution.y, resolution.z);
                bumpTextureGenerator.setParameter("bumpTexture",bumpTexture.getTexture());
                bumpTextureGenerator.setParameter("texture",Shader::CurrentTexture);
                refractionTextureGenerator.setParameter("resolution",resolution.x, resolution.y, resolution.z);
                refractionTextureGenerator.setParameter("bumpTexture",bumpTexture.getTexture());
                backgroundColor = sf::Color::Transparent;
            } else {
                throw core::Erreur(55, "Shader not supported!", 0);
            }
        }
        void OITRenderComponent::pushEvent(window::IEvent event, RenderWindow& rw) {
            if (event.type == window::IEvent::WINDOW_EVENT && event.window.type == window::IEvent::WINDOW_EVENT_RESIZED && &getWindow() == &rw && isAutoResized()) {
                std::cout<<"recompute size"<<std::endl;
                recomputeSize();
                getListener().pushEvent(event);
                getView().reset(physic::BoundingBox(getView().getViewport().getPosition().x, getView().getViewport().getPosition().y, getView().getViewport().getPosition().z, event.window.data1, event.window.data2, getView().getViewport().getDepth()));
            }
        }
        bool OITRenderComponent::needToUpdate() {
            return update;
        }
        void OITRenderComponent::changeVisibleEntities(Entity* toRemove, Entity* toAdd, EntityManager* em) {
            bool removed;
            em->removeAnimatedVisibleEntity(toRemove, visibleEntities, view, removed);
            std::cout<<"removed : "<<removed<<std::endl;
            //if (removed) {
                em->insertAnimatedVisibleEntity(toAdd, visibleEntities, view);
                loadEntitiesOnComponent(visibleEntities);
                update = true;
            //}
        }
        std::string OITRenderComponent::getExpression() {
            return expression;
        }
        void OITRenderComponent::setBackgroundColor(sf::Color color) {
            this->backgroundColor = color;
        }
        void OITRenderComponent::clear() {
             frameBuffer.clear(backgroundColor);
             depthBuffer.clear(sf::Color::Transparent);
             frontBuffer.clear(sf::Color::Transparent);
             bumpTexture.clear(sf::Color::Transparent);
             refractionTexture.clear(sf::Color::Transparent);
             opaquePixels.clear(sf::Color::Transparent);
             semiTransparentPixels.clear(sf::Color::Transparent);
             weightedBlendedOITPass1.clear(sf::Color::Transparent);
             weightedBlendedOITPass2.clear(sf::Color::White);
        }
        Sprite& OITRenderComponent::getFrameBufferTile () {
            return frameBufferTile;
        }
        Sprite& OITRenderComponent::getDepthBufferTile() {
            return depthBufferTile;
        }
        Sprite& OITRenderComponent::getOpaquePixelsTile () {
            return opaquePixelsTile;
        }
        Sprite& OITRenderComponent::getSemiTransparentPixelsTile() {
            return semiTransparentsPixelsTile;
        }
        Sprite& OITRenderComponent::getWeightedBlendedOITPass1Tile() {
            return weightedBlendedOITPass1Tile;
        }
        Sprite& OITRenderComponent::getWeightedBlendedOITPass2Tile() {
            return weightedBlendedOITPass2Tile;
        }
        const Texture& OITRenderComponent::getDepthBufferTexture() {
            return depthBuffer.getTexture();
        }
        const Texture& OITRenderComponent::getFrameBufferTexture() {
            return frameBuffer.getTexture();
        }
        const Texture& OITRenderComponent::getFrontBufferTexture() {
            return frontBuffer.getTexture();
        }
        bool OITRenderComponent::loadEntitiesOnComponent(std::vector<Entity*> vEntities)
        {
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
        void OITRenderComponent::setView(View view) {
            this->view = view;
            frameBuffer.setView(getWindow().getView());
            depthBuffer.setView(view);
            frontBuffer.setView(view);
            bumpTexture.setView(view);
            refractionTexture.setView(view);
            opaquePixels.setView(view);
            semiTransparentPixels.setView(view);
            weightedBlendedOITPass1.setView(view);
            weightedBlendedOITPass2.setView(view);
        }
        void OITRenderComponent::setExpression(std::string expression) {
            update = true;
            this->expression = expression;
        }
        void OITRenderComponent::drawNextFrame() {
            /*update = false;
            currentStates.blendMode = sf::BlendNone;
            for (unsigned int i = 0; i < m_instances.size(); i++) {
                 if (m_instances[i].getMaterial().getTexture() == nullptr)
                    depthBufferGenerator.setParameter("haveTexture", 0);
                 else
                    depthBufferGenerator.setParameter("haveTexture", 1);
                 currentStates.texture=m_instances[i].getMaterial().getTexture();
                 currentStates.shader=&depthBufferGenerator;
                 depthBuffer.draw(m_instances[i].getAllVertices(),currentStates);
                 currentStates.shader=nullptr;
                 frontBuffer.draw(m_instances[i].getAllVertices(), currentStates);
            }
            frontBuffer.display();
            frontBufferTile.setCenter(view.getPosition());
            frameBuffer.draw(frontBufferTile, currentStates);
            currentStates.shader=&frameBufferGenerator;
            currentStates.blendMode=sf::BlendMode(sf::BlendMode::Factor::OneMinusDstAlpha, sf::BlendMode::Factor::DstAlpha, sf::BlendMode::Equation::Add, sf::BlendMode::Factor::OneMinusDstAlpha, sf::BlendMode::Factor::One, sf::BlendMode::Equation::Add);
            for (unsigned int i = 0; i < m_instances.size(); i++) {
                if (m_instances[i].getMaterial().getTexture() == nullptr)
                    frameBufferGenerator.setParameter("haveTexture", 0);
                 else
                    frameBufferGenerator.setParameter("haveTexture", 1);
                currentStates.texture = m_instances[i].getMaterial().getTexture();
                frameBuffer.draw(m_instances[i].getAllVertices(), currentStates);
            }*/
            for (unsigned int i = 0; i < m_instances.size(); i++) {
                if (m_instances[i].getAllVertices().getVertexCount() > 0) {
                     if (m_instances[i].getMaterial().getTexture() == nullptr) {
                        depthBufferGenerator.setParameter("haveTexture", 0.f);
                        filterNotOpaquePixels.setParameter("haveTexture", 0.f);
                        generateWeightedBlendedOITPass1.setParameter("haveTexture", 0.f);
                        generateWeightedBlendedOITPass2.setParameter("haveTexture", 0.f);
                     } else {
                        depthBufferGenerator.setParameter("haveTexture", 1.f);
                        filterNotOpaquePixels.setParameter("haveTexture", 1.f);
                        generateWeightedBlendedOITPass1.setParameter("haveTexture", 1.f);
                        generateWeightedBlendedOITPass2.setParameter("haveTexture", 1.f);
                     }
                     currentStates.blendMode = sf::BlendNone;
                     currentStates.texture=m_instances[i].getMaterial().getTexture();
                     currentStates.shader=&depthBufferGenerator;
                     depthBuffer.draw(m_instances[i].getAllVertices(),currentStates);
                     currentStates.blendMode = sf::BlendAlpha;
                     currentStates.shader=&filterNotOpaquePixels;
                     opaquePixels.draw(m_instances[i].getAllVertices(), currentStates);
                     currentStates.blendMode = sf::BlendMode(sf::BlendMode::Factor::One, sf::BlendMode::Factor::One,sf::BlendMode::Equation::Add);
                     currentStates.shader = &generateWeightedBlendedOITPass1;
                     //weightedBlendedOITPass1.draw(m_instances[i].getAllVertices(), currentStates);
                     currentStates.blendMode = sf::BlendMode(sf::BlendMode::Factor::Zero, sf::BlendMode::Factor::OneMinusSrcAlpha,sf::BlendMode::Equation::Add);
                     currentStates.shader = &generateWeightedBlendedOITPass2;
                     //weightedBlendedOITPass2.draw(m_instances[i].getAllVertices(), currentStates);
                }
            }
            for (unsigned int i = 0; i < m_instances.size(); i++) {
                if (m_instances[i].getAllVertices().getVertexCount() > 0) {
                     if (m_instances[i].getMaterial().getTexture() == nullptr) {
                        filterOpaquePixels.setParameter("haveTexture", 0.f);
                     } else {
                        filterOpaquePixels.setParameter("haveTexture", 1.f);
                     }
                     sf::BlendMode blendMode(sf::BlendMode::Factor::OneMinusSrcAlpha, sf::BlendMode::Factor::SrcAlpha,sf::BlendMode::Equation::Add);
                     currentStates.shader = &filterOpaquePixels;
                     currentStates.blendMode = blendMode;
                     semiTransparentPixels.draw(m_instances[i].getAllVertices(), currentStates);
                }
            }
            for (unsigned int i = 0; i < drawables.size(); i++) {
                frameBuffer.draw(drawables[i].first.get(), drawables[i].second);
            }
            opaquePixels.display();
            semiTransparentPixels.display();
            currentStates.blendMode = sf::BlendAlpha;
            currentStates.shader = nullptr;
            opaquePixelsTile.setCenter(getWindow().getView().getPosition());
            semiTransparentsPixelsTile.setCenter(getWindow().getView().getPosition());
            frameBuffer.draw(opaquePixelsTile, currentStates);
            frameBuffer.draw(semiTransparentsPixelsTile, currentStates);
            depthBuffer.display();
            frameBuffer.display();
            bumpTexture.display();
            weightedBlendedOITPass1.display();
            weightedBlendedOITPass2.display();
            refractionTexture.display();
        }
        std::vector<Entity*> OITRenderComponent::getEntities() {
            return visibleEntities;
        }
        void OITRenderComponent::draw(Drawable& drawable, RenderStates states) {
            update = true;
            drawables.push_back(std::make_pair(std::ref(drawable), states));
        }
        void OITRenderComponent::draw(RenderTarget& target, RenderStates states) {
            frameBufferTile.setCenter(target.getView().getPosition());
            target.draw(frameBufferTile, states);
            drawables.clear();
        }

        View& OITRenderComponent::getView() {
            return view;
        }
        int OITRenderComponent::getLayer() {
            return getPosition().z;
        }
        const Texture& OITRenderComponent::getBumpTexture() {
            return bumpTexture.getTexture();
        }
        const Texture& OITRenderComponent::getRefractionTexture() {
            return refractionTexture.getTexture();
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
