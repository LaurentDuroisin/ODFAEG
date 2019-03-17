#include "../../../include/odfaeg/Graphics/map.h"
#include "../../../include/odfaeg/Graphics/rectangleShape.h"
#include "../../../include/odfaeg/Physics/boundingEllipsoid.h"
#include <iostream>
#include <climits>
#include "../../../include/odfaeg/Core/singleton.h"
#include "../../../include/odfaeg/Graphics/tGround.h"
#include "../../../include/odfaeg/Graphics/boneAnimation.hpp"
namespace odfaeg {
    namespace graphic {
    using namespace std;

        Map::Map (RenderComponentManager* frcm, std::string name, int cellWidth, int cellHeight, int cellDepth) : EntityManager(name), frcm(frcm) {
            gridMap = new GridMap(cellWidth, cellHeight);
            updateComponents = false;
            id = 0;
            version = 1;
            this->name = name;
            diagSize = math::Math::sqrt(math::Math::power(cellWidth, 2) + math::Math::power(cellHeight, 2));
            sf::Vector3i resolution;
            if (frcm != nullptr) {
                View view = frcm->getWindow().getView();
                physic::BoundingBox viewArea = view.getViewVolume();
                math::Vec3f position (viewArea.getPosition().x,viewArea.getPosition().y, view.getPosition().z);
                math::Vec3f size (viewArea.getWidth(), viewArea.getHeight(), 0);
                if (Shader::isAvailable()) {
                    shadowMap = std::make_unique<RenderTexture>();
                    lightMap = std::make_unique<RenderTexture>();
                    stencilBuffer = std::make_unique<RenderTexture>();
                    normalMap = std::make_unique<RenderTexture>();
                    refractionMap = std::make_unique<RenderTexture>();
                    backDepthBuffer = std::make_unique<RenderTexture>();
                    shadowMap->create(frcm->getWindow().getSize().x, frcm->getWindow().getSize().y,frcm->getWindow().getSettings());
                    lightMap->create(frcm->getWindow().getSize().x, frcm->getWindow().getSize().y,frcm->getWindow().getSettings());
                    window::ContextSettings settings = frcm->getWindow().getSettings();
                    settings.depthBits = 32;
                    stencilBuffer->create(frcm->getWindow().getSize().x, frcm->getWindow().getSize().y,settings);
                    normalMap->create(frcm->getWindow().getSize().x, frcm->getWindow().getSize().y,frcm->getWindow().getSettings());
                    refractionMap->create(frcm->getWindow().getSize().x, frcm->getWindow().getSize().y,frcm->getWindow().getSettings());
                    backDepthBuffer->create(frcm->getWindow().getSize().x, frcm->getWindow().getSize().y,frcm->getWindow().getSettings());
                    resolution = sf::Vector3i ((int) frcm->getWindow().getSize().x, (int) frcm->getWindow().getSize().y, frcm->getWindow().getView().getSize().z);
                    perPixLightingShader = std::make_unique<Shader>();
                    buildShadowMapShader = std::make_unique<Shader>();
                    perPixShadowShader = std::make_unique<Shader>();
                    buildNormalMapShader = std::make_unique<Shader>();
                    buildRefractionMapShader = std::make_unique<Shader>();
                    depthBufferGenShader = std::make_unique<Shader>();
                    perPixShadowShader2 = std::make_unique<Shader>();
                    shadowTile = std::make_unique<Tile> (&shadowMap->getTexture(),math::Vec3f(position.x, position.y, position.z), math::Vec3f(size.x, size.y, 0),sf::IntRect(0, 0, size.x, size.y));
                    lightTile = std::make_unique<Tile>  (&lightMap->getTexture(), math::Vec3f(position.x, position.y, position.z), math::Vec3f(size.x, size.y, 0),sf::IntRect(0, 0, size.x, size.y));
                    refractionTile = std::make_unique<Tile> (&refractionMap->getTexture(), math::Vec3f(position.x, position.y, position.z), math::Vec3f(size.x, size.y, 0),sf::IntRect(0, 0, size.x, size.y));
                    stencilBufferTile = std::make_unique<Tile> (&stencilBuffer->getTexture(), math::Vec3f(position.x, position.y, position.z), math::Vec3f(size.x, size.y, 0),sf::IntRect(0, 0, size.x, size.y));
                    const std::string  depthGenVertexShader =
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
                    "uniform sampler2D texture;"
                    "uniform vec3 resolution;"
                    "uniform float haveTexture;"
                    "in mat4 projMat;"
                    "mat4 inverse(mat4 mat) {"
                    "   mat4 inv;"
                    "   return inv;"
                    "}"
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
                     const std::string perPixLightingVertexShader =
                     "#version 130 \n"
                     "out mat4 projMat;"
                     "void main () {"
                          "gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
                          "gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;"
                          "gl_FrontColor = gl_Color;"
                          "projMat = gl_ProjectionMatrix;"
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
                     "mat4 inverse(mat4 mat) {"
                     "   mat4 inv;"
                     "   return inv;"
                     "}"
                     "void main () { "
                         "vec2 position = vec2 (gl_FragCoord.xy / resolution.xy);"
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
                    const std::string buildShadowMapVertexShader =
                    "#version 130 \n"
                    "uniform mat4 shadowProjMat;"
                    "out mat4 projMat;"
                    "void main () {"
                        "gl_Position = gl_ModelViewProjectionMatrix * shadowProjMat * gl_Vertex;"
                        "gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;"
                        "gl_FrontColor = gl_Color;"
                        "projMat = gl_ProjectionMatrix;"
                    "}";
                    const std::string buildShadowMapFragmentShader =
                    "#version 130 \n"
                    "uniform sampler2D texture;"
                    "uniform float haveTexture;"
                    "in mat4 projMat;"
                    "mat4 inverse(mat4 mat) {"
                    "   mat4 inv;"
                    "   return inv;"
                    "}"
                    "void main() {"
                    "   vec4 color = (haveTexture == 1) ? texture2D(texture, gl_TexCoord[0].xy) * gl_Color : gl_Color;"
                    "   float z = (gl_FragCoord.w != 1.f) ? (inverse(projMat) * vec4(0, 0, 0, gl_FragCoord.w)).w : gl_FragCoord.z;"
                    "   gl_FragColor = vec4(0, 0, z, color.a);"
                    "}";
                    const std::string perPixShadowVertexShader =
                    "#version 130 \n"
                    "uniform mat4 depthBiasMatrix;"
                    "uniform mat4 shadowProjMat;"
                    "out vec4 shadowCoords;"
                    "out mat4 projMat;"
                    "void main () {"
                        "gl_Position = gl_ProjectionMatrix * shadowProjMat * gl_ModelViewMatrix * gl_Vertex;"
                        "gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;"
                        "gl_FrontColor = gl_Color;"
                        "projMat = gl_ProjectionMatrix;"
                        "shadowCoords = depthBiasMatrix * vec4(gl_Position.xyz, 1);"
                    "}";
                    const std::string perPixShadowVertexShader2 =
                    "#version 130 \n"
                    "uniform mat4 depthBiasMatrix;"
                    "out vec4 shadowCoords;"
                    "void main () {"
                        "gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
                        "gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;"
                        "gl_FrontColor = gl_Color;"
                        "shadowCoords = depthBiasMatrix * vec4(gl_Position.xyz, 1);"
                    "}";
                    const std::string perPixShadowFragmentShader =
                    "#version 130 \n"
                    "uniform sampler2D texture;"
                    "uniform sampler2D stencilBuffer;"
                    "uniform float set;"
                    "uniform float haveTexture;"
                    "in vec4 shadowCoords;"
                    "in mat4 projMat;"
                    "mat4 inverse(mat4 mat) {"
                    "   mat4 inv;"
                    "   return inv;"
                    "}"
                    "void main() {"
                    "   vec4 color = (haveTexture == 1) ? texture2D(texture, gl_TexCoord[0].xy) * gl_Color : gl_Color;"
                    "   vec4 stencil = texture2D (stencilBuffer, shadowCoords.xy);"
                    "   float z = (gl_FragCoord.w != 1.f) ? (inverse(projMat) * vec4(0, 0, 0, gl_FragCoord.w)).w : gl_FragCoord.z;"
                    "   if (stencil.z < z && set == 1) {"
                    "       gl_FragColor = vec4(0, 0, 0, color.a);"
                    "   } else {"
                    "       gl_FragColor = vec4(1, 1, 1, 1);"
                    "   }"
                    "}";
                    const std::string perPixShadowFragmentShader2 =
                    "#version 130 \n"
                    "uniform sampler2D texture;"
                    "uniform sampler2D shadowBuffer;"
                    "uniform float set;"
                    "uniform float haveTexture;"
                    "in vec4 shadowCoords;"
                    "void main() {"
                    "   vec4 color = (haveTexture == 1) ? texture2D(texture, gl_TexCoord[0].xy) * gl_Color : gl_Color;"
                    "   vec4 shadow = texture2D (shadowBuffer, shadowCoords.xy);"
                    "   if (set == 1) {"
                    "       gl_FragColor = vec4(1, 1, 1, color.a);"
                    "   } else {"
                    "       gl_FragColor = shadow;"
                    "   }"
                    "}";
                    const std::string buildRefractionMapVertexShader =
                    "#version 130 \n"
                    "out mat4 projMat;"
                    "void main () {"
                        "gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
                        "gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;"
                        "gl_FrontColor = gl_Color;"
                        "projMat = gl_ProjectionMatrix;"
                    "}";
                    const std::string buildRefractionMapFragmentShader =
                    "#version 130 \n"
                    "uniform vec3 resolution;"
                    "uniform sampler2D texture;"
                    "uniform sampler2D refractionTexture;"
                    "uniform sampler2D refractionBuffer;"
                    "uniform sampler2D framebuffer;"
                    "uniform sampler2D backDepthBuffer;"
                    "uniform float haveTexture;"
                    "in mat4 projMat;"
                    "mat4 inverse(mat4 mat) {"
                    "   mat4 inv;"
                    "   return inv;"
                    "}"
                    "void main() {"
                        "vec2 position = vec2 (gl_FragCoord.xy / resolution.xy);"
                        "vec4 refraction = texture2D(refractionTexture, position);"
                        "vec4 previous_depth_alpha = texture2D(backDepthBuffer, position);"
                        "vec4 previous_color = texture2D(refractionBuffer, position);"
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
                        "colors[0].a = previous_color.a + current_color.a * (1 - previous_color.a);"
                        "b = (current_depth >= previous_depth_alpha.z);"
                        "vec4 backfbcolor = colors[int(b)];"
                        "vec4 fbcolor = texture2D(framebuffer, position);"
                        "float z = (gl_FragCoord.w != 1.f) ? (inverse(projMat) * vec4(0, 0, 0, gl_FragCoord.w)).w : gl_FragCoord.z;"
                        "if (z > refraction.z && refraction.x != 0) {"
                            "gl_FragColor = (fbcolor * fbcolor.a + backfbcolor * (1 - fbcolor.a)) * refraction.x;"
                        "} else {"
                            "gl_FragColor = previous_color;"
                        "}"
                    "}";
                    if (!buildNormalMapShader->loadFromMemory(buildNormalMapVertexShader, buildNormalMapFragmentShader)) {
                        throw core::Erreur(52, "Error, failed to load build normal map shader", 3);
                    }
                    if (!buildShadowMapShader->loadFromMemory(buildShadowMapVertexShader, buildShadowMapFragmentShader)) {
                        throw core::Erreur(53, "Error, failed to load build shadow map shader", 3);
                    }
                    if (!perPixShadowShader->loadFromMemory(perPixShadowVertexShader, perPixShadowFragmentShader)) {
                        throw core::Erreur(54, "Error, failed to load per pix shadow map shader", 3);
                    }
                    if (!perPixShadowShader2->loadFromMemory(perPixShadowVertexShader2, perPixShadowFragmentShader2)) {
                        throw core::Erreur(55, "Error, failed to load per pix shadow map shader", 3);
                    }
                    if (!perPixLightingShader->loadFromMemory(perPixLightingVertexShader, perPixLightingFragmentShader)) {
                        throw core::Erreur(56, "Error, failed to load per pixel lighting shader", 3);
                    }
                    if (!buildRefractionMapShader->loadFromMemory(buildRefractionMapVertexShader, buildRefractionMapFragmentShader)) {
                        throw core::Erreur(57, "Error, failed to load refraction shader", 3);
                    }
                    if (!depthBufferGenShader->loadFromMemory(depthGenVertexShader, depthGenFragShader)) {
                        throw core::Erreur(58, "Error, failed to load depth buffer generator shader", 3);
                    }
                    buildNormalMapShader->setParameter("texture", Shader::CurrentTexture);
                    perPixLightingShader->setParameter("resolution", resolution.x, resolution.y, resolution.z);
                    perPixLightingShader->setParameter("lightMap",lightMap->getTexture());
                    buildShadowMapShader->setParameter("texture", Shader::CurrentTexture);
                    perPixShadowShader->setParameter("stencilBuffer", stencilBuffer->getTexture());
                    perPixShadowShader->setParameter("texture", Shader::CurrentTexture);
                    perPixShadowShader2->setParameter("shadowBuffer", shadowMap->getTexture());
                    perPixShadowShader2->setParameter("texture", Shader::CurrentTexture);
                    buildRefractionMapShader->setParameter("texture", Shader::CurrentTexture);
                    buildRefractionMapShader->setParameter("resolution", resolution.x, resolution.y, resolution.z);
                    buildRefractionMapShader->setParameter("refractionBuffer", refractionMap->getTexture());
                    buildRefractionMapShader->setParameter("backdepthBuffer", backDepthBuffer->getTexture());
                    buildRefractionMapShader->setParameter("refractionBuffer", refractionMap->getTexture());
                    depthBufferGenShader->setParameter("resolution", resolution.x, resolution.y, resolution.z);
                    depthBufferGenShader->setParameter("texture", Shader::CurrentTexture);
                    depthBufferGenShader->setParameter("depthBuffer", backDepthBuffer->getTexture());
                } else {
                    throw core::Erreur(55, "Shader not supported!", 0);
                }
            }
        }
        void Map::generate_map(std::vector<Tile*> tGround, std::vector<Tile*> walls, math::Vec2f tileSize, physic::BoundingBox &rect, bool terrain3D) {

            int startX = rect.getPosition().x / tileSize.x * tileSize.x;
            int startY = rect.getPosition().y / tileSize.y * tileSize.y;
            int endX = (rect.getPosition().x + rect.getWidth()) / tileSize.x * tileSize.x;
            int endY = (rect.getPosition().y + rect.getHeight()) / tileSize.y * tileSize.y;
            BigTile *bt;
            if (!terrain3D)
                bt = new BigTile(math::Vec3f(startX, startY, startY + endY * 0.5f));
            else
                bt = new BigTile(math::Vec3f(startX, startY, rect.getPosition().z),tileSize,rect.getWidth() / tileSize.x);
            //Positions de d\E9part et d'arriv\E9es en fonction de la taille, de la position et de la taille des cellules de la map.
            for (int y = startY; y < endY;  y+=tileSize.y) {
                for (int x = startX; x < endX; x+=tileSize.x) {

                    //On projete les positions en fonction de la projection du jeux.
                    math::Vec3f projPos = gridMap->getBaseChangementMatrix().changeOfBase(math::Vec3f (x - startX, y - startY, 0));
                    math::Vec2f pos (projPos.x + startX, projPos.y + startY);
                    //Mur du coin en haut \E0 gauche.
                    if (x == startX && y == startY && walls.size() >= 4) {
                        g2d::Wall *w = new g2d::Wall(new Tile(*walls[3]),&g2d::AmbientLight::getAmbientLight());
                        w->setPosition(math::Vec3f(pos.x, pos.y, pos.y + walls[3]->getSize().y * 0.5f));
                        addEntity(w);
                        gridMap->getGridCellAt(math::Vec3f(w->getPosition().x, w->getPosition().y, 0))->setPassable(false);

                        //Mur du coin en haut \E0 droite.
                    } else if (x == endX - tileSize.x && y == startY && walls.size() >= 5) {
                        g2d::Wall *w = new g2d::Wall(new Tile(*walls[4]),&g2d::AmbientLight::getAmbientLight());
                        w->setPosition(math::Vec3f(pos.x, pos.y, pos.y + walls[4]->getSize().y * 0.5f));
                        addEntity(w);
                        gridMap->getGridCellAt(math::Vec3f(w->getPosition().x, w->getPosition().y, w->getPosition().z))->setPassable(false);
                        //Mur du coin en bas \E0 droite.
                    } else if (x == endX - tileSize.x && y == endY - tileSize.y && walls.size() >= 3) {
                        g2d::Wall *w = new g2d::Wall(new Tile(*walls[2]),&g2d::AmbientLight::getAmbientLight());
                        w->setPosition(math::Vec3f(pos.x, pos.y, pos.y + walls[2]->getSize().y * 0.5f));
                        addEntity(w);
                        gridMap->getGridCellAt(math::Vec3f(w->getPosition().x, w->getPosition().y, 0))->setPassable(false);
                    } else if (x == startX && y == endY - tileSize.y && walls.size() >= 6) {
                        g2d::Wall *w = new g2d::Wall(new Tile(*walls[5]),&g2d::AmbientLight::getAmbientLight());
                        w->setPosition(math::Vec3f(pos.x, pos.y, pos.y + walls[5]->getSize().y * 0.5f));
                        addEntity(w);
                        gridMap->getGridCellAt(math::Vec3f(w->getPosition().x, w->getPosition().y, 0))->setPassable(false);
                    } else if ((y == startY || y == endY - tileSize.y) && walls.size() >= 1) {
                        g2d::Wall *w = new g2d::Wall(new Tile(*walls[0]),&g2d::AmbientLight::getAmbientLight());
                        w->setPosition(math::Vec3f(pos.x, pos.y, pos.y + walls[0]->getSize().y * 0.5f));
                        addEntity(w);
                        if (y == endY - tileSize.y) {
                            int i = math::Math::random(tGround.size());
                            Tile *tile = new Tile(*tGround[i]);
                            tile->setPosition(math::Vec3f(pos.x, pos.y, pos.y + tile->getSize().y * 0.5f));
                            bt->addTile(tile);
                        }
                        gridMap->getGridCellAt(math::Vec3f(w->getPosition().x, w->getPosition().y, 0))->setPassable(false);
                    } else if ((x == startX || x == endX - tileSize.x) && walls.size() >= 2) {
                        g2d::Wall *w = new g2d::Wall(new Tile(*walls[1]),&g2d::AmbientLight::getAmbientLight());
                        w->setPosition(math::Vec3f(pos.x, pos.y, pos.y + walls[1]->getSize().y * 0.5f));
                        addEntity(w);
                        if (x == endX - tileSize.x) {
                            int i = math::Math::random(tGround.size());
                            Tile *tile = new Tile(*tGround[i]);
                            tile->setPosition(math::Vec3f(pos.x, pos.y, pos.y + tile->getSize().y * 0.5f));
                            bt->addTile(tile);
                        }
                        gridMap->getGridCellAt(math::Vec3f(w->getPosition().x, w->getPosition().y, 0))->setPassable(false);
                    } else {
                        Tile *tile;
                        if (tGround.size() > 0)  {
                            int i = math::Math::random(tGround.size());
                            tile = new Tile(*tGround[i]);
                            tile->setPosition(math::Vec3f(pos.x, pos.y, pos.y + tile->getSize().y * 0.5f));
                        } else {
                            tile = new Tile(nullptr, math::Vec3f(pos.x, pos.y, pos.y + tileSize.y * 0.5f), math::Vec3f(tileSize.x, tileSize.y, 0), sf::IntRect(0, 0, tileSize.x, tileSize.y));
                        }
                        if (terrain3D) {
                            float heights[4];
                            for (unsigned int j = 0; j < sizeof(heights) / sizeof(float); j++) {
                                heights[j] = math::Math::random(rect.getPosition().z, rect.getPosition().z + rect.getDepth());
                            }
                            bt->addTile(tile, math::Vec2f(pos.x, pos.y), heights);
                        } else {
                            bt->addTile(tile);
                        }
                    }
                }
            }
            addEntity(bt);
        }
        vector<math::Vec2f> Map::getPath(Entity* entity, math::Vec2f finalPos) {
            return gridMap->getPath(entity, finalPos);
        }
        void Map::setName (string name) {
            this->name = name;
        }
        string Map::getName() {
            return name;
        }
        void Map::setId (int id) {
            this->id = id;
        }
        int Map::getId () {
            return id;
        }
        void Map::setVersion (int version) {
            this->version = version;
        }
        int Map::getVersion () {
            return version;
        }
        void Map::removeComptImg (const void* resource) {
            map<const void*, int>::iterator it;
            it = compImages.find(resource);
            if (it != compImages.end()) {
                compImages.erase(it);
            }
        }
        void Map::increaseComptImg(const void* resource) {
            map<const void*, int>::iterator it;
            it = compImages.find(resource);
            if (it != compImages.end()) {
                it->second = it->second + 1;
            } else {
                compImages.insert(pair<const void*, int> (resource, 1));
            }
        }
        void Map::decreaseComptImg (const void* resource) {
            map<const void*, int>::iterator it;
            it = compImages.find(resource);
            if (it != compImages.end() && it->second != 0) {
                it->second = it->second - 1;
            }
        }
        int Map::getCompImage(const void* resource) {
            map<const void*, int>::iterator it;
            it = compImages.find(resource);
            if (it != compImages.end())
                return it->second;
            return -1;

        }
        bool Map::addEntity(Entity *entity) {
            if (entity->isAnimated()) {
                if (static_cast<AnimatedEntity*>(entity)->getCurrentFrame() != nullptr) {
                    addEntity(static_cast<AnimatedEntity*>(entity)->getCurrentFrame());
                } else {
                    gridMap->addEntity(entity);
                }
            } else {
                std::vector<Entity*> children = entity->getChildren();
                for (unsigned int i = 0; i < children.size(); i++) {
                     addEntity(children[i]);
                }
            }
            /*if (entity->getType() == "E_PONCTUAL_LIGHT") {
                    std::cout<<"ponctual light"<<std::endl;
                    sf:sleep(sf::seconds(1));
            }*/
            if (entity->isLeaf()) {
                for (unsigned int j = 0; j < entity->getFaces().size(); j++) {
                     if (entity->getFaces()[j]->getMaterial().getTexture() != nullptr) {
                         increaseComptImg(entity->getFaces()[j]->getMaterial().getTexture());
                     }
                }
                gridMap->addEntity(entity);
            }
            /*std::vector<Entity*> tiles;
            getChildren(entity, tiles, "*");
            if (tiles.size() != 0) {
                for (unsigned int i = 0; i < tiles.size(); i++) {
                   for (unsigned int j = 0; j < tiles[i]->getFaces().size(); j++) {
                        if (tiles[i]->getFaces()[j]->getMaterial().getTexture() != nullptr) {
                            increaseComptImg(tiles[i]->getFaces()[j]->getMaterial().getTexture());
                        }
                   }
                }
            } else {
               for (unsigned int j = 0; j < entity->getFaces().size(); j++) {
                    if (entity->getFaces()[j]->getMaterial().getTexture() != nullptr) {
                        increaseComptImg(entity->getFaces()[j]->getMaterial().getTexture());
                    }
               }
            }
            return gridMap->addEntity(entity);*/
        }
        bool Map::removeEntity (Entity *entity) {
            std::vector<Entity*> children = entity->getChildren();
            for (unsigned int i = 0; i < children.size(); i++) {
                if (entity->isAnimated()) {
                    removeEntity(static_cast<AnimatedEntity*>(children[i])->getCurrentFrame());
                } else {
                    removeEntity(children[i]);
                }
            }
            if (entity->isLeaf()) {
                std::vector<Face*> faces = entity->getFaces();
                for (unsigned int j = 0; j < faces.size(); j++) {
                    decreaseComptImg(faces[j]->getMaterial().getTexture());
                }
                gridMap->removeEntity(entity);
            }
            /*std::vector<Entity*> tiles;
            getChildren(entity, tiles, "*");
            for (unsigned int i = 0; i < tiles.size(); i++) {
               for (unsigned int j = 0; j < tiles[i]->getFaces().size(); j++) {
                   decreaseComptImg(tiles[i]->getFaces()[j]->getMaterial().getTexture());
               }
            }
            return gridMap->removeEntity(entity);*/
        }
        bool Map::deleteEntity (Entity *entity) {
            std::vector<Entity*> children = entity->getChildren();
            for (unsigned int i = 0; i < children.size(); i++) {
                if (entity->isAnimated()) {
                    removeEntity(static_cast<AnimatedEntity*>(children[i])->getCurrentFrame());
                } else {
                    removeEntity(children[i]);
                }
            }
            if (entity->isLeaf()) {
                std::vector<Face*> faces = entity->getFaces();
                for (unsigned int j = 0; j < faces.size(); j++) {
                    decreaseComptImg(faces[j]->getMaterial().getTexture());
                }
                gridMap->deleteEntity(entity);
            }
            /*std::vector<Entity*> tiles;
            getChildren(entity, tiles, "*");
            for (unsigned int i = 0; i < tiles.size(); i++) {
               for (unsigned int j = 0; j < tiles[i]->getFaces().size(); j++) {
                   decreaseComptImg(tiles[i]->getFaces()[j]->getMaterial().getTexture());
               }
            }
            return gridMap->deleteEntity(entity);*/
        }
        math::Vec3f Map::getPosition() {
            return gridMap->getMins();
        }
        int Map::getWidth() {
            return gridMap->getSize().x;
        }
        int Map::getHeight() {
            return gridMap->getSize().y;
        }
        int Map::getNbCasesPerRow () {
            return gridMap->getNbCasesPerRow();
        }
        bool Map::removeEntity(int id) {
            return gridMap->deleteEntity(id);
        }
        void Map::rotateEntity(Entity *entity, int angle) {
            removeEntity(entity);
            entity->setRotation(angle);
            addEntity(entity);
            /*gridMap->removeEntity(entity);
            entity->setRotation(angle);
            gridMap->replaceEntity(entity);*/
        }
        void Map::scaleEntity(Entity *entity, float sx, float sy) {
            removeEntity(entity);
            entity->setScale(math::Vec3f(sx, sy, 0));
            addEntity(entity);
            /*gridMap->removeEntity(entity);
            entity->setScale(math::Vec3f(sx, sy, 0));
            gridMap->replaceEntity(entity);*/
        }
        void Map::moveEntity(Entity *entity, float dx, float dy, float dz) {
            removeEntity(entity);
            entity->move(math::Vec3f(dx, dy, dz));
            addEntity(entity);
            /*gridMap->removeEntity(entity);
            entity->move(math::Vec3f(dx, dy, dz));
            gridMap->addEntity(entity);*/
        }
        void Map::checkVisibleEntities() {
            for (unsigned int c = 0; c < frcm->getNbComponents() + 1; c++) {
                if (c == frcm->getNbComponents() || c < frcm->getNbComponents() && frcm->getRenderComponent(c) != nullptr) {
                    physic::BoundingBox view;
                    if (c == frcm->getNbComponents())
                        view = frcm->getWindow().getView().getViewVolume();
                    else
                        view = frcm->getRenderComponent(c)->getView().getViewVolume();
                    visibleParentEntities.clear();
                    vEntitiesByType.clear();
                    visibleEntities.clear();
                    visibleEntities.resize(Entity::getNbEntityTypes());
                    for (unsigned int i = 0; i < visibleEntities.size(); i++) {
                        visibleEntities[i].resize(Entity::getNbEntities());
                        std::fill(visibleEntities[i].begin(),visibleEntities[i].end() , nullptr);
                    }
                    int x = view.getPosition().x;
                    int y = view.getPosition().y;
                    int z = view.getPosition().z;
                    int endX = view.getPosition().x + view.getWidth() + 100;
                    int endY = view.getPosition().y + view.getHeight() + 100;
                    int endZ = view.getDepth() + 100;
                    physic::BoundingBox bx (x, y, z, endX-view.getPosition().x, endY-view.getPosition().y, endZ);

                    for (int i = x; i <= endX; i+=gridMap->getOffsetX()) {
                        for (int j = y; j <= endY; j+=gridMap->getOffsetY()) {
                            //for (int k = 0; k <= endZ; k+=gridMap->getOffsetZ()) {
                                math::Vec3f point(i, j, 0);
                                CellMap* cell = getGridCellAt(point);
                                if (cell != nullptr) {
                                    for (unsigned int n = 0; n < cell->getNbEntitiesInside(); n++) {
                                       Entity* entity = cell->getEntityInside(n);
                                       /*if (entity->getType() == "E_PONCTUAL_LIGHT") {
                                            std::cout<<"ponctual light"<<std::endl;
                                            sf::sleep(sf::seconds(1.f));
                                       }*/
                                       //std::cout<<"entity : "<<entity->getPosition()<<entity->getSize()<<std::endl;
                                       physic::BoundingBox& bounds = entity->getGlobalBounds();
                                       /*if (entity->getRootType() == "E_PONCTUAL_LIGHT")
                                            std::cout<<"add ponctual light"<<std::endl;*/
                                       if (bx.intersects(bounds) && visibleEntities[entity->getRootTypeInt()][entity->getId()] != entity) {

                                           visibleEntities[entity->getRootTypeInt()][entity->getId()] = entity;
                                       }
                                       /*if (!containsVisibleParentEntity(entity->getRootEntity())) {
                                            visibleParentEntities.push_back(entity->getRootEntity());
                                            insertVisibleEntity(entity, bx);
                                        }*/
                                    }
                                }
                            //}
                        }
                    }
                }
                if (c < frcm->getNbComponents() && frcm->getRenderComponent(c) != nullptr) {
                    std::vector<Entity*> entities = getVisibleEntities(frcm->getRenderComponent(c)->getExpression());
                    frcm->getRenderComponent(c)->loadEntitiesOnComponent(entities);
                }
            }
            /*std::string s;
            std::cin>>s;*/

        }
        bool Map::containsVisibleParentEntity(Entity* entity) {
            for (unsigned int i = 0; i < visibleParentEntities.size(); i++) {
                if (visibleParentEntities[i] == entity)
                    return true;
            }
            return false;
        }
        void Map::insertVisibleEntity(Entity *entity, physic::BoundingBox& bx) {
            if (!containsVisibleEntity(entity)) {
                physic::BoundingBox& bx2  = entity->getGlobalBounds();
                //std::cout<<"view volume : "<<bx2.getPosition()<<" "<<bx2.getWidth()<<" "<<bx2.getHeight()<<" "<<bx2.getDepth()<<std::endl;
                if (bx.intersects(bx2)) {
                    /*VEntitiesByType it = vEntitiesByType.find(entity->getRootType());
                    std::cout<<"root type : "<<entity->getRootType()<<std::endl;
                    if (it == vEntitiesByType.end()) {
                        std::cout<<"add new root type : "<<entity->getRootType()<<std::endl;
                        std::pair<std::string, std::vector<Entity*>> newEntitiesType(entity->getRootType(), std::vector<Entity*>());
                        vEntitiesByType.insert(newEntitiesType);
                        it = vEntitiesByType.find(entity->getRootType());
                    }
                    it->second.push_back(entity);*/
                    visibleEntities[entity->getRootTypeInt()].push_back(entity);
                }
            }
            /*if (entity->isAnimated()) {
                if (!containsVisibleEntity(entity)) {
                    VEntitiesByType it = vEntitiesByType.find(entity->getType());
                    if (it == vEntitiesByType.end()) {
                        std::pair<std::string, std::vector<Entity*>> newEntitiesType(entity->getType(), std::vector<Entity*>());
                        vEntitiesByType.insert(newEntitiesType);
                        it = vEntitiesByType.find(entity->getType());
                    }
                    it->second.push_back(entity);
                    insertVisibleEntity(static_cast<AnimatedEntity*>(entity)->getCurrentFrame(), bx);
                }
            } else {
               if (!containsVisibleEntity(entity)) {
                    physic::BoundingBox bx2 = entity->getGlobalBounds();
                    if (bx.intersects(bx2)) {
                        VEntitiesByType it = vEntitiesByType.find(entity->getType());
                        if (it == vEntitiesByType.end()) {
                            std::pair<std::string, std::vector<Entity*>> newEntitiesType(entity->getType(), std::vector<Entity*>());
                            vEntitiesByType.insert(newEntitiesType);
                            it = vEntitiesByType.find(entity->getType());
                        }
                        it->second.push_back(entity);
                    }
                }
                vector<Entity*> children;
                getChildren(entity, children, "*");
                for (unsigned int i = 0; i < children.size(); i++) {
                    if (!containsVisibleEntity(children[i])) {
                        physic::BoundingBox bx2  = children[i]->getGlobalBounds();
                        //std::cout<<"view volume : "<<bx2.getPosition()<<" "<<bx2.getWidth()<<" "<<bx2.getHeight()<<" "<<bx2.getDepth()<<std::endl;
                        if (bx.intersects(bx2)) {
                            VEntitiesByType it = vEntitiesByType.find(entity->getType());
                            if (it == vEntitiesByType.end()) {
                                std::pair<std::string, std::vector<Entity*>> newEntitiesType(entity->getType(), std::vector<Entity*>());
                                vEntitiesByType.insert(newEntitiesType);
                                it = vEntitiesByType.find(entity->getType());
                            }
                            it->second.push_back(children[i]);
                        }
                    }
                }
            }*/
        }
        void Map::setBaseChangementMatrix (BaseChangementMatrix bm) {
            gridMap->setBaseChangementMatrix(bm);
        }
        void Map::insertAnimatedVisibleEntity (Entity *ae, std::vector<Entity*>& entities, View& view) {
            if (containsVisibleParentEntity(ae->getRootEntity())) {
                if (ae->isAnimated()) {
                    insertAnimatedVisibleEntity(static_cast<AnimatedEntity*> (ae)->getCurrentFrame(), entities, view);
                } else {
                    physic::BoundingBox bx (view.getViewVolume().getPosition().x,view.getViewVolume().getPosition().y,view.getViewVolume().getPosition().z,view.getViewVolume().getWidth(), view.getViewVolume().getHeight(), view.getViewVolume().getDepth());
                    vector<Entity*> children;
                    getChildren(ae, children, "*");
                    if (!containsVisibleEntity(ae)) {
                        std::cout<<"doesn't contains visible entity"<<std::endl;
                        physic::BoundingBox bx2 = ae->getGlobalBounds();
                        if (bx.intersects(bx2)) {
                            std::cout<<"add visible entity"<<std::endl;
                            entities.push_back(ae);
                        }
                    }
                    for (unsigned int i = 0; i < children.size(); i++) {
                        if (!containsVisibleEntity(children[i])) {
                            physic::BoundingBox bx2 = children[i]->getGlobalBounds();
                            if (bx.intersects(bx2)) {
                                entities.push_back(children[i]);
                            }
                        }
                    }
                }
            }
        }
        void Map::changeVisibleEntity(Entity* toRemove, Entity* toAdd) {
            /*View view = frcm->getWindow().getView();
            physic::BoundingBox bx (view.getViewVolume().getPosition().x,view.getViewVolume().getPosition().y,view.getViewVolume().getPosition().z,view.getViewVolume().getWidth(), view.getViewVolume().getHeight(), view.getViewVolume().getDepth());
            VEntitiesByType it = vEntitiesByType.find(toRemove->getType());
            std::vector<Entity*>::iterator it2;
            vector<Entity*> children;
            getChildren(toRemove, children, "*");
            for (it2 = it->second.begin(); it2 != it->second.end();) {
                if (*it2 == toRemove) {
                    it2 = it->second.erase(it2);
                } else {
                    it2++;
                }
            }
            for (unsigned int i = 0; i < children.size(); i++) {
                for (it2 = it->second.begin(); it2 != it->second.end();) {
                    if (*it2 == children[i]) {
                        it2 = it->second.erase(it2);
                    } else {
                        it2++;
                    }
                }
            }
            getChildren(toAdd, children, "*");
            it = vEntitiesByType.find(toAdd->getType());
            if (!containsVisibleEntity(toAdd)) {
                physic::BoundingBox bx2 = toAdd->getGlobalBounds();
                if (bx.intersects(bx2)) {
                    it->second.push_back(toAdd);
                }
            } else {
                for (unsigned int i = 0; i < children.size(); i++) {
                    if (!containsVisibleEntity(children[i])) {
                        physic::BoundingBox bx2 = children[i]->getGlobalBounds();
                        if (bx.intersects(bx2)) {
                            it->second.push_back(children[i]);
                        }
                    }
                }
            }
            for (unsigned int i = 0; i < frcm->getNbComponents(); i++) {
                if (frcm->getRenderComponent(i) != nullptr) {
                    frcm->getRenderComponent(i)->changeVisibleEntities(toRemove, toAdd, this);
                }
            }*/
            for (unsigned int i = 0; i < frcm->getNbComponents(); i++) {
                if (frcm->getRenderComponent(i) != nullptr) {
                    frcm->getRenderComponent(i)->loadEntitiesOnComponent(frcm->getRenderComponent(i)->getEntities());
                }
            }
        }
        void Map::removeAnimatedVisibleEntity(Entity *toRemove, std::vector<Entity*>& entities, View& view, bool& removed) {
            if (toRemove->isAnimated()) {
                removeAnimatedVisibleEntity(static_cast<AnimatedEntity*>(toRemove)->getCurrentFrame(), entities, view, removed);
            } else {
                vector<Entity*>::iterator it2;
                vector<Entity*> children;
                getChildren(toRemove, children, "*");
                for (it2 = entities.begin(); it2 != entities.end();) {
                    if (*it2 == toRemove) {
                        removed = true;
                        it2 = entities.erase(it2);
                    } else {
                        it2++;
                    }
                }
                for (unsigned int i = 0; i < children.size(); i++) {
                    for (it2 = entities.begin(); it2 != entities.end();) {
                        if (*it2 == children[i]) {
                            removed = true;
                            it2 = entities.erase(it2);
                        } else {
                            it2++;
                        }
                    }
                }
            }
        }
        Entity* Map::getEntity(int id) {
            return gridMap->getEntity(id);
        }
        vector<CellMap*> Map::getCasesMap() {
            return gridMap->getCasesMap();
        }

        void Map::getChildren (Entity *entity, std::vector<Entity*>& entities, std::string type) {
            vector<Entity*> children = entity->getChildren();
            for (unsigned int i = 0; i < children.size(); i++) {
                if (children[i]->getChildren().size() != 0)
                    getChildren(children[i], children, type);
                if (type.size() > 0 && type.at(0) == '*') {
                    std::string types;
                    if (type.find("-") != string::npos)
                        types = type.substr(2, type.size() - 3);
                    vector<string> excl = core::split(types, "-");
                    bool exclude = false;
                    for (unsigned int j = 0; j < excl.size(); j++) {
                        if (children[i]->getType() == excl[j])
                            exclude = true;
                    }
                    if (!exclude) {
                        entities.push_back(children[i]);
                    }
                } else {
                   vector<string> types = core::split(type, "+");
                   for (unsigned int j = 0; j < types.size(); j++) {
                        if (children[i]->getType() == types[j]) {
                            entities.push_back(children[i]);
                        }
                   }
                }
            }
        }

        vector<Entity*> Map::getEntities(string type) {
            vector<Entity*> entities;
            vector<Entity*> allEntities = gridMap->getEntities();
            if (type.size() > 0 && type.at(0) == '*') {
                if (type.find("-") != string::npos)
                    type = type.substr(2, type.size() - 3);
                vector<string> excl = core::split(type, "-");
                for (unsigned int i = 0; i < allEntities.size(); i++) {
                    Entity* entity = allEntities[i]->getRootEntity();
                    bool exclude = false;
                    for (unsigned int j = 0; j < excl.size(); j++) {
                        if (entity->getRootType() == excl[i])
                            exclude = true;
                    }
                    if (!exclude) {
                        bool contains = false;
                        for (unsigned int n = 0; n < entities.size() && !contains; n++) {
                            if (entities[n]->getRootEntity() == entity->getRootEntity()) {
                                contains = true;
                            }
                        }
                        if (!contains) {
                            entity->getRootEntity()->updateTransform();
                            entities.push_back(entity->getRootEntity());
                        }
                    }
                }
                return entities;
            }
            vector<string> types = core::split(type, "+");
            for (unsigned int i = 0; i < types.size(); i++) {
                for (unsigned int j = 0; j < allEntities.size(); j++) {
                    Entity* entity = allEntities[j]->getRootEntity();
                    if (entity->getRootType() == types[i]) {
                        bool contains = false;
                        for (unsigned int n = 0; n < entities.size() && !contains; n++) {
                            if (entities[n]->getRootEntity() == entity->getRootEntity()) {
                                contains = true;
                            }
                        }
                        if (!contains) {
                            entity->getRootEntity()->updateTransform();
                            entities.push_back(entity->getRootEntity());
                        }
                    }
                }
            }
            return entities;
        }

        vector<Entity*> Map::getVisibleEntities (std::string type) {
            std::vector<Entity*> entities;
            if (type.size() > 0 && type.at(0) == '*') {
                if (type.find("-") != string::npos)
                    type = type.substr(2, type.size() - 2);
                vector<string> excl = core::split(type, "-");
                for (unsigned int i = 0; i < visibleEntities.size(); i++) {
                    for (unsigned int j = 0; j < visibleEntities[i].size(); j++) {
                        if (visibleEntities[i][j] != nullptr) {
                            bool exclude = false;
                            for (unsigned int t = 0; t < excl.size(); t++) {
                                if (visibleEntities[i][j]->getRootType() == excl[t])
                                    exclude = true;
                            }
                            if (!exclude) {
                                BoneAnimation* ba = dynamic_cast<BoneAnimation*>(visibleEntities[i][j]->getRootEntity());
                                if (ba != nullptr) {
                                    if (ba->getBoneIndex() == visibleEntities[i][j]->getBoneIndex()) {
                                        entities.push_back(visibleEntities[i][j]);
                                    }
                                } else {
                                    entities.push_back(visibleEntities[i][j]);
                                }
                            }
                        }
                    }
                }
                return entities;
            }
            vector<string> types = core::split(type, "+");
            for (unsigned int t = 0; t < types.size(); t++) {
                vector<Entity*> visibleEntitiesType = visibleEntities[Entity::getIntOfType(types[t])];
                for (unsigned int i = 0; i < visibleEntitiesType.size(); i++) {
                    if (visibleEntitiesType[i] != nullptr) {
                        BoneAnimation* ba = dynamic_cast<BoneAnimation*>(visibleEntitiesType[i]->getRootEntity());
                        if (ba != nullptr) {
                            if (ba->getBoneIndex() == visibleEntitiesType[i]->getBoneIndex()) {
                                entities.push_back(visibleEntitiesType[i]);
                            }
                        } else {
                            entities.push_back(visibleEntitiesType[i]);
                        }
                    }
                }
            }
            return entities;
        }

        vector<Entity*> Map::getEntitiesInBox (physic::BoundingBox bx, std::string type) {
             vector<Entity*> entities;
             vector<Entity*> allEntitiesInRect = gridMap->getEntitiesInBox(bx);

             if (type.at(0) == '*') {
                if (type.find("-") != string::npos)
                    type = type.substr(2, type.size() - 3);
                vector<string> excl = core::split(type, "-");
                for (unsigned int i = 0; i < allEntitiesInRect.size(); i++) {
                    Entity* entity = allEntitiesInRect[i];
                    bool exclude = false;
                    for (unsigned int i = 0; i < excl.size(); i++) {
                        if (entity->getRootType() == excl[i])
                            exclude = true;
                    }
                    if (!exclude) {
                        entities.push_back(entity);
                    }
                }
                vector<string> types = core::split(type, "+");
                for (unsigned int i = 0; i < types.size(); i++) {
                    for (unsigned int j = 0; j < allEntitiesInRect.size(); j++) {
                        Entity* entity = allEntitiesInRect[j];
                        if (entity->getRootType() == types[i]) {
                            entities.push_back(entity);
                        }

                    }
                }
                return entities;
            }
            vector<string> types = core::split(type, "+");
            for (unsigned int i = 0; i < types.size(); i++) {
                 for (unsigned int j = 0; j < allEntitiesInRect.size(); j++) {
                    Entity* entity = allEntitiesInRect[j];
                    if (entity->getRootType() == types[i])
                        entities.push_back(entity);
                }
            }
            return entities;
        }
        math::Vec3f Map::getCoordinatesAt(math::Vec3f p) {
            math::Vec2f c(p.x, p.y);
            return gridMap->getCoordinatesAt(c);
        }
        CellMap* Map::getGridCellAt(math::Vec3f p) {
            return gridMap->getGridCellAt(p);
        }
        vector<CellMap*> Map::getCasesInBox (physic::BoundingBox bx) {
            return gridMap->getCasesInBox(bx);
        }
        bool Map::containsVisibleEntity(Entity* entity) {
            std::vector<Entity*> visibleEntitiesType = visibleEntities[entity->getRootTypeInt()];
            for (unsigned int i = 0; i < visibleEntitiesType.size(); i++) {
                if (visibleEntitiesType[i] == entity) {
                    return true;
                }
            }
            return false;
        }
        bool Map::collide (Entity *entity, math::Vec3f position) {
             return gridMap->collideWithEntity(entity, position);
        }
        bool Map::collide (Entity *entity) {
             return gridMap->collideWithEntity(entity);
        }
        bool Map::collide (Entity* entity, math::Ray ray) {
             math::Vec3f point = ray.getOrig() + ray.getDir().normalize() * diagSize * 0.001f;
             math::Vec3f v1 = ray.getExt() - ray.getOrig();
             math::Vec3f v2 = point - ray.getOrig();
             while (v2.magnSquared() / v1.magnSquared() < 1) {
                    if (collide(entity, point))
                        return true;
                    point += ray.getDir().normalize() * diagSize * 0.001f;
                    v2 = point - ray.getOrig();
             }
             point = ray.getExt();
             return collide(entity, point);
        }

        void Map::drawOnComponents(std::string expression, int layer, sf::BlendMode blendMode) {
            HeavyComponent* frc = frcm->getRenderComponent(layer);
            if (frc != nullptr) {
                frc->setExpression(expression);
            }
        }
        void Map::drawOnComponents(Drawable& drawable, int layer, RenderStates states) {
            HeavyComponent *frc = frcm->getRenderComponent(layer);
            if (frc != nullptr) {
                frc->draw(drawable, states);
            }
        }
        void Map::generateStencilBuffer(std::string expression, int n, va_list args) {
            if (Shader::isAvailable()) {
                std::vector<Entity*> entities;
                View view = frcm->getWindow().getView();
                physic::BoundingBox viewArea = view.getViewVolume();
                math::Vec3f position (viewArea.getPosition().x,viewArea.getPosition().y, view.getPosition().z);
                math::Vec3f size (viewArea.getWidth(), viewArea.getHeight(), 0);
                stencilBuffer->setView(frcm->getWindow().getView());
                stencilBuffer->clear(sf::Color::Transparent);
                RenderStates states;
                states.shader = buildShadowMapShader.get();
                //states.blendMode = sf::BlendNone;
                if (n != -1) {
                    std::vector<unsigned int> idsCompInt;
                    for (int c = 0; c < n; c++) {
                        int j = va_arg(args, int);
                        if (j < n)
                            idsCompInt.push_back(j);
                    }
                    for (unsigned int i = 0; i < frcm->getNbComponents(); i++) {
                        if (frcm->getRenderComponent(i) != nullptr) {
                            entities = frcm->getRenderComponent(i)->getEntities();
                            view = frcm->getRenderComponent(i)->getView();
                            math::Vec3f centerLight = g2d::AmbientLight::getAmbientLight().getLightCenter();
                            view = View(view.getSize().x, view.getSize().y, -g2d::AmbientLight::getAmbientLight().getHeight(), g2d::AmbientLight::getAmbientLight().getHeight());
                            view.setCenter(centerLight);
                            math::Vec3f forward = frcm->getRenderComponent(i)->getView().getPosition() - view.getPosition();
                            math::Vec3f target = view.getPosition() + forward;
                            view.lookAt(target.x, target.y, target.z);
                            stencilBuffer->setView(view);
                            bool find = false;
                            math::Vec3f v = view.getPosition() - frcm->getRenderComponent(i)->getView().getPosition();
                            math::Matrix4f biasMatrix(0.5f, 0.0f, 0.0f, 0.0f,
                                                      0.0f, 0.5f, 0.0f, 0.0f,
                                                      0.0f, 0.0f, 0.5f, 0.0f,
                                                      0.5f, 0.5f, 0.5f, 1.f);
                            math::Matrix4f depthBiasMatrix = biasMatrix * view.getViewMatrix().getMatrix() * view.getProjMatrix().getMatrix();
                            perPixShadowShader->setParameter("depthBiasMatrix", depthBiasMatrix.transpose());
                            for (unsigned int k = 0; k < entities.size(); k++) {
                                /*forward = entities[k]->getCenter() - view.getPosition();
                                target = view.getPosition() + forward;
                                view.lookAt(target.x, target.y, target.z);*/
                                if (entities[k]->getFaces().size() > 0) {
                                    if (entities[k]->getFaces()[0]->getMaterial().getTexture() != nullptr) {
                                        buildShadowMapShader->setParameter("haveTexture", 1);
                                    } else {
                                        buildShadowMapShader->setParameter("haveTexture", 0);
                                    }
                                    math::Vec3f shadowOrigin, shadowCenter, shadowScale(1.f, 1.f, 1.f), shadowRotationAxis;
                                    float shadowRotationAngle = 0;
                                    if (entities[k]->getParent() != nullptr && entities[k]->getParent()->isModel()) {
                                        shadowCenter = static_cast<Model*>(entities[k]->getParent())->getShadowCenter();
                                        shadowScale = static_cast<Model*>(entities[k]->getParent())->getShadowScale();
                                        shadowRotationAxis = static_cast<Model*>(entities[k]->getParent())->getShadowRotationAxis();
                                        shadowRotationAngle = static_cast<Model*>(entities[k]->getParent())->getShadowRotationAngle();
                                        shadowOrigin = static_cast<Model*>(entities[k]->getParent())->getShadowOrigin();
                                    }
                                    TransformMatrix tm;
                                    tm.setOrigin(shadowOrigin);
                                    tm.setScale(shadowScale);
                                    tm.setRotation(shadowRotationAxis, shadowRotationAngle);
                                    tm.setTranslation(shadowOrigin + shadowCenter);
                                    tm.update();
                                    buildShadowMapShader->setParameter("shadowProjMat", tm.getMatrix().transpose());
                                    stencilBuffer->draw(*entities[k], states);
                                }
                            }
                        }
                    }
                }
                va_end(args);
                stencilBuffer->display();
                stencilBufferTile->setPosition(position);
            }
        }

        Entity& Map::getShadowTile(std::string expression, int n, va_list args) {
            std::vector<Entity*> entities;
            View view = frcm->getWindow().getView();
            physic::BoundingBox viewArea = view.getViewVolume();
            math::Vec3f position (viewArea.getPosition().x,viewArea.getPosition().y, view.getPosition().z);
            math::Vec3f size (viewArea.getWidth(), viewArea.getHeight(), 0);
            shadowMap->clear(sf::Color::White);
            shadowMap->setView(frcm->getWindow().getView());
            if (Shader::isAvailable()) {
                RenderStates states;
                //states.blendMode = sf::BlendNone;
                if (n != -1) {
                    std::vector<unsigned int> idsCompInt;
                    for (int c = 0; c < n; c++) {
                        int j = va_arg(args, int);
                        if (j < n)
                            idsCompInt.push_back(j);
                    }
                    for (unsigned int i = 0; i < frcm->getNbComponents(); i++) {
                        if (frcm->getRenderComponent(i) != nullptr) {
                            entities = frcm->getRenderComponent(i)->getEntities();
                            view = frcm->getRenderComponent(i)->getView();
                            shadowMap->setView(view);
                            bool find = false;
                            for (unsigned int k = 0; k < idsCompInt.size(); k++)
                                if (idsCompInt[k] == i)
                                    find = true;
                            if (find) {
                                //std::cout<<"don't set"<<std::endl;
                                perPixShadowShader->setParameter("set", 1);
                                perPixShadowShader2->setParameter("set", 1);
                            } else {
                                //std::cout<<"set"<<std::endl;
                                perPixShadowShader->setParameter("set", 0);
                                perPixShadowShader2->setParameter("set", 1);
                            }
                            for (unsigned int p = 0; p < 2; p++) {
                                for (unsigned int k = 0; k < entities.size(); k++) {
                                    if (entities[k]->getFaces().size() > 0) {
                                        if (entities[k]->getFaces()[0]->getMaterial().getTexture() != nullptr) {
                                            perPixShadowShader->setParameter("haveTexture", 1);
                                            perPixShadowShader2->setParameter("haveTexture", 1);
                                        } else {
                                            perPixShadowShader->setParameter("haveTexture", 0);
                                            perPixShadowShader2->setParameter("haveTexture", 0);
                                        }
                                        if (p == 0) {
                                            math::Vec3f shadowCenter, shadowScale(1.f, 1.f, 1.f), shadowRotationAxis;
                                            float shadowRotationAngle = 0;
                                            shadowCenter = entities[k]->getShadowCenter();
                                            shadowScale = entities[k]->getShadowScale();
                                            shadowRotationAxis = entities[k]->getShadowRotationAxis();
                                            shadowRotationAngle = entities[k]->getShadowRotationAngle();
                                            TransformMatrix tm;
                                            tm.setOrigin(entities[k]->getPosition());
                                            tm.setScale(shadowScale);
                                            tm.setRotation(shadowRotationAxis, shadowRotationAngle);
                                            tm.setTranslation(entities[k]->getPosition() + shadowCenter);
                                            tm.update();
                                            perPixShadowShader->setParameter("shadowProjMat", tm.getMatrix().transpose());
                                            states.shader = perPixShadowShader.get();
                                            shadowMap->draw(*entities[k], states);
                                        } else {
                                            states.shader = perPixShadowShader2.get();
                                            shadowMap->draw(*entities[k], states);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                RectangleShape rect(size * 2.f);
                rect.setPosition(position - size * 0.5f);
                rect.setFillColor(sf::Color(100, 100, 100, 128));
                shadowMap->draw(rect);
                shadowMap->display();
            }
            va_end(args);
            shadowTile->setPosition(position);
            return *shadowTile;
        }
        Entity& Map::getLightTile (std::string expression, int n, va_list args) {
            std::vector<Entity*> lights = getVisibleEntities(expression);
            View view = frcm->getWindow().getView();
            physic::BoundingBox viewArea = view.getViewVolume();
            math::Vec3f position (viewArea.getPosition().x,viewArea.getPosition().y, view.getPosition().z);
            math::Vec3f size (viewArea.getWidth(), viewArea.getHeight(), 0);
            sf::Color ambientColor = g2d::AmbientLight::getAmbientLight().getColor();
            lightMap->clear(sf::Color::Black);
            lightMap->setView(frcm->getWindow().getView());
            RectangleShape rect(size * 2.f);
            rect.setPosition(position - size * 0.5f);
            rect.setFillColor(ambientColor);
            lightMap->draw(rect);
            lightMap->display();
            if (Shader::isAvailable()) {
                RenderStates states;
                if (n != -1) {
                       std::vector<unsigned int> idsCompInt;
                       for (int c = 0; c < n; c++) {
                           int j = va_arg(args, int);
                           if (j < n)
                               idsCompInt.push_back(j);
                       }
                       for (unsigned int i = 0; i < frcm->getNbComponents(); i++) {
                            if (frcm->getRenderComponent(i) != nullptr) {
                                bool find = false;
                                for (unsigned int k = 0; k < idsCompInt.size(); k++)
                                    if (idsCompInt[k] == i)
                                        find = true;
                                if (find) {
                                    lightMap->setView(frcm->getRenderComponent(i)->getView());
                                    normalMap->setView(frcm->getRenderComponent(i)->getView());
                                    normalMap->clear(sf::Color::Transparent);
                                    if (dynamic_cast<OITRenderComponent*>(frcm->getRenderComponent(i)) != nullptr) {
                                        Sprite& heightMapTile = static_cast<OITRenderComponent*>(frcm->getRenderComponent(i))->getDepthBufferTile();
                                        heightMapTile.setCenter(frcm->getRenderComponent(i)->getView().getPosition());
                                        states.shader = buildNormalMapShader.get();
                                        states.blendMode = sf::BlendNone;
                                        normalMap->draw(heightMapTile, states);
                                        normalMap->display();
                                        //perPixLightingShader->setParameter("specularTexture", static_cast<OITRenderComponent*>(frcm->getRenderComponent(i))->getSpecularTexture());
                                        perPixLightingShader->setParameter("normalMap", normalMap->getTexture());
                                        perPixLightingShader->setParameter("bumpMap", static_cast<OITRenderComponent*>(frcm->getRenderComponent(i))->getBumpTexture());
                                        states.shader = perPixLightingShader.get();
                                        states.blendMode = sf::BlendAdd;
                                        for (unsigned int j = 0; j < lights.size(); j++) {
                                            EntityLight* el = static_cast<EntityLight*> (lights[j]);
                                            math::Vec3f center = frcm->getWindow().mapCoordsToPixel(el->getLightCenter(), frcm->getRenderComponent(i)->getView());
                                            center.w = el->getSize().x * 0.5f;
                                            perPixLightingShader->setParameter("lightPos", center.x, center.y, center.z, center.w);
                                            perPixLightingShader->setParameter("lightColor", el->getColor().r, el->getColor().g,el->getColor().b,el->getColor().a);
                                            lightMap->draw(*el, states);
                                        }
                                    }
                                }
                           }
                       }
                       lightMap->display();
                }
            }
            va_end(args);
            lightTile->setPosition(position);
            return *lightTile;
        }
        Entity& Map::getRefractionTile (std::string expression, int n, va_list args) {
            std::vector<Entity*> entities = getVisibleEntities(expression);
            View view = frcm->getWindow().getView();
            physic::BoundingBox viewArea = view.getViewVolume();
            math::Vec3f position (viewArea.getPosition().x,viewArea.getPosition().y, view.getPosition().z);
            math::Vec3f size (viewArea.getWidth(), viewArea.getHeight(), 0);
            refractionMap->clear(sf::Color::Black);
            refractionMap->setView(frcm->getWindow().getView());
            backDepthBuffer->clear(sf::Color::Black);
            backDepthBuffer->setView(frcm->getWindow().getView());
            RenderStates states;
            states.shader = buildRefractionMapShader.get();
            if (Shader::isAvailable()) {
                if (n != -1) {
                       std::vector<unsigned int> idsCompInt;
                       for (int c = 0; c < n; c++) {
                           int j = va_arg(args, int);
                           if (j < n)
                               idsCompInt.push_back(j);
                       }
                       for (unsigned int i = 0; i < frcm->getNbComponents(); i++) {
                            if (frcm->getRenderComponent(i) != nullptr) {
                                bool find = false;
                                for (unsigned int k = 0; k < idsCompInt.size(); k++)
                                    if (idsCompInt[k] == i)
                                        find = true;
                                if (find) {
                                    View view = frcm->getRenderComponent(i)->getView();
                                    math::Vec3f forward = view.getForward();
                                    view.lookAt(-forward.x, -forward.y, -forward.z);
                                    refractionMap->setView(view);
                                    backDepthBuffer->setView(view);
                                    if (dynamic_cast<OITRenderComponent*>(frcm->getRenderComponent(i)) != nullptr) {
                                        buildRefractionMapShader->setParameter("refractionTexture",static_cast<OITRenderComponent*>(frcm->getRenderComponent(i))->getRefractionTexture());
                                        buildRefractionMapShader->setParameter("frameBuffer",static_cast<OITRenderComponent*>(frcm->getRenderComponent(i))->getFrameBufferTexture());
                                        for (unsigned int k = 0; k < entities.size(); k++) {
                                            if (entities[k]->getFaces().size() > 0) {
                                                if (entities[k]->getFaces()[0]->getMaterial().getTexture() != nullptr) {
                                                    perPixShadowShader->setParameter("haveTexture", 1);
                                                    depthBufferGenShader->setParameter("haveTexture", 1);
                                                } else {
                                                    perPixShadowShader->setParameter("haveTexture", 0);
                                                    depthBufferGenShader->setParameter("haveTexture", 0);
                                                }
                                            }
                                            states.shader = buildRefractionMapShader.get();
                                            refractionMap->draw(*entities[k], states);
                                            states.shader = depthBufferGenShader.get();
                                            backDepthBuffer->draw(*entities[k], states);
                                        }
                                    }
                                }
                            }
                       }
                }
            }
            refractionMap->display();
            refractionTile->setPosition(position);
            return *refractionTile;
        }
        BaseChangementMatrix Map::getBaseChangementMatrix() {
            return gridMap->getBaseChangementMatrix();
        }
        void Map::updateParticles() {
            for (unsigned int i = 0; i < frcm->getNbComponents(); i++) {
                if (frcm->getRenderComponent(i) != nullptr) {
                    frcm->getRenderComponent(i)->updateParticleSystems();
                }
            }
        }
    }
}

