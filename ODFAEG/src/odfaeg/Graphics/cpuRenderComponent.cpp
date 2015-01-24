#include "../../../include/odfaeg/Graphics/cpuRenderComponent.hpp"
namespace odfaeg {
    namespace graphic {
        CPURenderComponent::CPURenderComponent(math::Vec3f position,math::Vec3f size, math::Vec3f origin, bool useThread, RenderWindow& rw)
        : Component(position,size, origin, useThread), window(rw), view (view) {
            frameBuffer.resize(rw.getSize().x * rw.getSize().y * 4);
            depthBuffer.resize(rw.getSize().x * rw.getSize().y);
            this->size.x = rw.getSize().x;
            this->size.y = rw.getSize().y;
            view = window.getView();
        }
        void CPURenderComponent::loadEntitiesOnComponent(std::vector<Entity*> vEntities) {
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
        }
        void CPURenderComponent::clear() {
            for (unsigned int x = 0; x < size.x; x++) {
                for (unsigned int y = 0; y < size.y; y++) {
                    frameBuffer[(y * size.x + x) * 4] = 0;
                    frameBuffer[(y * size.x + x) * 4 + 1] = 0;
                    frameBuffer[(y * size.x + x) * 4 + 2] = 0;
                    frameBuffer[(y * size.x + x) * 4 + 3] = 255;
                    depthBuffer[y * size.x + x] = math::Vec3f(0, 0, 0, 1);
                }
            }
        }
        void CPURenderComponent::draw(RenderTarget& target, RenderStates states) {
            Tile tile(&fbTexture, math::Vec3f(0, 0, 0),math::Vec3f(size.x, size.y, 0), sf::IntRect(0, 0, size.x, size.y));
            tile.setCenter(view.getPosition());
            target.draw(tile, states);
        }
        void CPURenderComponent::raytrace() {
             for (unsigned int i = 0; i < m_instances.size(); i++) {
                for (unsigned int j = 0; j < m_instances[i]->getVertexArrays().size(); j++) {
                    TransformMatrix& tm = m_instances[i]->getTransforms()[j].get();
                    for (unsigned int k = 0; k < m_instances[i]->getVertexArrays()[j]->getVertexCount(); k++) {
                        math::Matrix4f texM;
                        math::Matrix4f invTexM;
                        if (m_instances[i]->getMaterial().getTexture() != nullptr) {
                            texM = m_instances[i]->getMaterial().getTexture()->getTextureMatrix();
                            invTexM = texM.inverse();
                        }
                        if (m_instances[i]->getVertexArrays()[j]->getPrimitiveType() == sf::TrianglesFan
                            && m_instances[i]->getVertexArrays()[j]->getVertexCount() >= 3
                            && k < m_instances[i]->getVertexArrays()[j]->getVertexCount() - 2) {
                            math::Vec3f p1 = math::Vec3f((*m_instances[i]->getVertexArrays()[j])[0].position.x,(*m_instances[i]->getVertexArrays()[j])[0].position.y,(*m_instances[i]->getVertexArrays()[j])[0].position.z);
                            math::Vec3f p2 = math::Vec3f((*m_instances[i]->getVertexArrays()[j])[k+1].position.x,(*m_instances[i]->getVertexArrays()[j])[k+1].position.y,(*m_instances[i]->getVertexArrays()[j])[k+1].position.z);
                            math::Vec3f p3 = math::Vec3f((*m_instances[i]->getVertexArrays()[j])[k+2].position.x,(*m_instances[i]->getVertexArrays()[j])[k+2].position.y,(*m_instances[i]->getVertexArrays()[j])[k+2].position.z);
                            p1 = tm.transform(p1);
                            p2 = tm.transform(p2);
                            p3 = tm.transform(p3);
                            math::Vec3f orig = math::Vec3f(view.getPosition().x, view.getPosition().y, view.getViewport().getPosition().z);
                            for (unsigned int x = 0; x < size.x; x++) {
                                for (unsigned int y = 0; y < size.y; y++) {
                                   math::Vec3f ext = window.mapPixelToCoords(math::Vec3f(x, y, view.getViewport().getPosition().z + view.getViewport().getSize().z), view);
                                   math::Ray ray(orig, ext);
                                   math::Vec3f o, point;
                                   math::Vec3f r = ray.getOrig() - ray.getExt();
                                   math::Vec3f u = p2 - p1;
                                   math::Vec3f v = p3 - p1;
                                   math::Vec3f n = u.cross(v);
                                   math::Vec3f otr = ray.getOrig() - p1;
                                   point.x = n.dot2(otr) / n.dot2(r);
                                   point.y = -otr.cross(u).dot2(r) / n.dot2(r);
                                   point.z = -v.cross(otr).dot2(r) / n.dot2(r);
                                   if (0 <= point.x
                                   && 0 <= point.y && point.y <= 1
                                   &&  0 <= point.z && point.z <= 1
                                   && point.y + point.z <= 1) {
                                       //std::cout<<"intersects"<<std::endl;
                                        sf::Color c1 = (*m_instances[i]->getVertexArrays()[j])[0].color;
                                        sf::Color c2 = (*m_instances[i]->getVertexArrays()[j])[k+1].color;
                                        sf::Color c3 = (*m_instances[i]->getVertexArrays()[j])[k+2].color;
                                        math::Vec3f ct1 = texM * math::Vec3f((*m_instances[i]->getVertexArrays()[j])[0].texCoords.x,(*m_instances[i]->getVertexArrays()[j])[0].texCoords.y, 1.f);
                                        math::Vec3f ct2 = texM * math::Vec3f((*m_instances[i]->getVertexArrays()[j])[k+1].texCoords.x,(*m_instances[i]->getVertexArrays()[j])[k+1].texCoords.y, 1.f);
                                        math::Vec3f ct3 = texM * math::Vec3f((*m_instances[i]->getVertexArrays()[j])[k+2].texCoords.x,(*m_instances[i]->getVertexArrays()[j])[k+2].texCoords.y, 1.f);
                                        math::Vec3f z (p1.z, p2.z, p3.z);
                                        float bz = z.x * point.x + z.y * point.y + z.z * point.z;
                                        float actualZ = depthBuffer[y * size.x + x].z;
                                        if (bz >= view.getViewport().getPosition().z && bz <= view.getViewport().getSize().z) {
                                            math::Vec3f r = math::Vec3f (c1.r / 255.f, c2.r / 255.f, c3.r / 255.f);
                                            math::Vec3f g = math::Vec3f (c1.g / 255.f, c2.g / 255.f, c3.g / 255.f);
                                            math::Vec3f b = math::Vec3f (c1.b / 255.f, c2.b / 255.f, c3.b / 255.f);
                                            math::Vec3f a = math::Vec3f (c1.a / 255.f, c2.a / 255.f, c3.a / 255.f);
                                            std::array<math::Vec3f, 2> colors;
                                            colors[0] = math::Vec3f(r.x * point.x + r.y * point.y + r.z * point.z,
                                                                    g.x * point.x + g.y * point.y + g.z * point.z,
                                                                    b.x * point.x + b.y * point.y + b.z * point.z,
                                                                    a.x * point.x + a.y * point.y + a.z * point.z);
                                            math::Vec3f finalColor = colors[0];
                                            depthBuffer[y * size.x + x] = math::Vec3f(0, 0, bz, colors[0].w);
                                            frameBuffer[(y * size.x + x) * 4] = finalColor.x * 255;
                                            frameBuffer[(y * size.x + x) * 4 + 1] = finalColor.y * 255;
                                            frameBuffer[(y * size.x + x) * 4 + 2] = finalColor.z * 255;
                                            frameBuffer[(y * size.x + x) * 4 + 3] = finalColor.w * 255;
                                        }
                                   }
                                }
                            }
                            winFrameBuffer.create(size.x, size.y, &frameBuffer[0]);
                            fbTexture.loadFromImage(winFrameBuffer);
                        }
                    }
                }
             }
        }
        void CPURenderComponent::drawNextFrame() {
            for (unsigned int i = 0; i < m_instances.size(); i++) {
                for (unsigned int j = 0; j < m_instances[i]->getVertexArrays().size(); j++) {
                    TransformMatrix& tm = m_instances[i]->getTransforms()[j].get();
                    for (unsigned int k = 0; k < m_instances[i]->getVertexArrays()[j]->getVertexCount(); k++) {
                        math::Matrix4f texM;
                        math::Matrix4f invTexM;
                        if (m_instances[i]->getMaterial().getTexture() != nullptr) {
                            texM = m_instances[i]->getMaterial().getTexture()->getTextureMatrix();
                            invTexM = texM.inverse();
                        }
                        if (m_instances[i]->getVertexArrays()[j]->getPrimitiveType() == sf::TrianglesFan
                            && m_instances[i]->getVertexArrays()[j]->getVertexCount() >= 3
                            && k < m_instances[i]->getVertexArrays()[j]->getVertexCount() - 2) {
                            math::Vec3f p1 = math::Vec3f((*m_instances[i]->getVertexArrays()[j])[0].position.x,(*m_instances[i]->getVertexArrays()[j])[0].position.y,(*m_instances[i]->getVertexArrays()[j])[0].position.z);
                            math::Vec3f p2 = math::Vec3f((*m_instances[i]->getVertexArrays()[j])[k+1].position.x,(*m_instances[i]->getVertexArrays()[j])[k+1].position.y,(*m_instances[i]->getVertexArrays()[j])[k+1].position.z);
                            math::Vec3f p3 = math::Vec3f((*m_instances[i]->getVertexArrays()[j])[k+2].position.x,(*m_instances[i]->getVertexArrays()[j])[k+2].position.y,(*m_instances[i]->getVertexArrays()[j])[k+2].position.z);
                            p1 = tm.transform(p1);
                            p2 = tm.transform(p2);
                            p3 = tm.transform(p3);
                            p1 = window.mapCoordsToPixel(p1, view);
                            p2 = window.mapCoordsToPixel(p2, view);
                            p3 = window.mapCoordsToPixel(p3, view);
                            sf::Color c1 = (*m_instances[i]->getVertexArrays()[j])[0].color;
                            sf::Color c2 = (*m_instances[i]->getVertexArrays()[j])[k+1].color;
                            sf::Color c3 = (*m_instances[i]->getVertexArrays()[j])[k+2].color;
                            math::Vec3f ct1 = texM * math::Vec3f((*m_instances[i]->getVertexArrays()[j])[0].texCoords.x,(*m_instances[i]->getVertexArrays()[j])[0].texCoords.y, 1.f);
                            math::Vec3f ct2 = texM * math::Vec3f((*m_instances[i]->getVertexArrays()[j])[k+1].texCoords.x,(*m_instances[i]->getVertexArrays()[j])[k+1].texCoords.y, 1.f);
                            math::Vec3f ct3 = texM * math::Vec3f((*m_instances[i]->getVertexArrays()[j])[k+2].texCoords.x,(*m_instances[i]->getVertexArrays()[j])[k+2].texCoords.y, 1.f);
                            std::array<math::Vec3f, 3> vertices = {math::Vec3f(p1.x, p1.y, 0),math::Vec3f(p2.x, p2.y, 0), math::Vec3f(p3.x, p3.y, 0)};
                            std::array<std::array<float, 2>, 3> extends = math::Computer::getExtends(vertices);
                            float minX = (extends[0][0] < 0) ? 0 : extends[0][0];
                            float minY = (extends[1][0] < 0) ? 0 : extends[1][0];
                            float maxX = (extends[0][1] >= size.x) ? size.x-1 : extends[0][1];
                            float maxY = (extends[1][0] >= size.y) ? size.y-1 : extends[1][1];
                            for (unsigned int x = minX; x < maxX; x++) {
                                for (unsigned int y = minY; y < maxY; y++) {
                                    math::Vec2f p(x, y);
                                    float det1 = (x - p1.x) * (p2.y - p1.y) - (y - p1.y) * (p2.x - p1.x);
                                    float det2 = (x - p2.x) * (p3.y - p2.y) - (y - p2.y) * (p3.x - p2.x);
                                    float det3 = (x - p3.x) * (p3.y - p1.y) - (y - p3.y) * (p3.x - p1.x);
                                    bool isInside = ((det1 <= 0 && det2 <= 0 && det3 >= 0)
                                                     || det1 >= 0 && det2 >= 0 && det3 <= 0) ? true : false;

                                    if (isInside) {

                                        math::Matrix2f m1(p1.x - p3.x, p2.x - p3.x,
                                                          p1.y - p3.y, p2.y - p3.y);
                                        float u = ((p2.y - p3.y) * (p.x - p3.x) + (p3.x - p2.x) * (p.y - p3.y)) / m1.getDet();
                                        float v = ((p3.y - p1.y) * (p.x - p3.x) + (p1.x - p3.x) * (p.y - p3.y)) / m1.getDet();
                                        float w = math::Math::abs(1 - u - v);
                                        math::Vec3f z (p1.z, p2.z, p3.z);
                                        float bz = z.x * u + z.y * v + z.z * w;
                                        float actualZ = depthBuffer[y * size.x + x].z;
                                        if (bz >= view.getViewport().getPosition().z && bz <= view.getViewport().getSize().z) {
                                            math::Vec3f tcx = math::Vec3f(ct1.x, ct2.x, ct3.x);
                                            math::Vec3f tcy = math::Vec3f(ct1.y, ct2.y, ct3.y);
                                            math::Vec3f tc = invTexM * math::Vec3f(tcx.x * u + tcx.y * v + tcx.z * w,
                                                                         tcy.x * u + tcy.y * v + tcy.z* w, 1.f);
                                            math::Vec3f texColor(1.f, 1.f, 1.f, 1.f);
                                            if (m_instances[i]->getMaterial().getTexture() != nullptr) {
                                                const sf::Image& texImg = m_instances[i]->getMaterial().getTexture()->getImage();
                                                sf::Color color = texImg.getPixel(tc.x, tc.y);
                                                texColor = math::Vec3f (color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f);
                                            }
                                            math::Vec3f r = math::Vec3f (c1.r / 255.f, c2.r / 255.f, c3.r / 255.f);
                                            math::Vec3f g = math::Vec3f (c1.g / 255.f, c2.g / 255.f, c3.g / 255.f);
                                            math::Vec3f b = math::Vec3f (c1.b / 255.f, c2.b / 255.f, c3.b / 255.f);
                                            math::Vec3f a = math::Vec3f (c1.a / 255.f, c2.a / 255.f, c3.a / 255.f);
                                            std::array<math::Vec3f, 2> colors;
                                            colors[0] = math::Vec3f(r.x * u + r.y * v + r.z * w,
                                                                    g.x * u + g.y * v + g.z * w,
                                                                    b.x * u + b.y * v + b.z * w,
                                                                    a.x * u + a.y * v + a.z * w) * texColor;
                                            colors[1] = math::Vec3f(frameBuffer[(y * size.x + x)*4] / 255.f,
                                                                               frameBuffer[(y * size.x + x)*4+1] / 255.f,
                                                                               frameBuffer[(y * size.x + x)*4+2] / 255.f,
                                                                               depthBuffer[y * size.x + x].w);
                                            bool src=(bz >= actualZ);
                                            math::Vec3f finalColor = colors[!src] * colors[!src].w + colors[src] * (1 - colors[!src].w);
                                            depthBuffer[y * size.x + x] = math::Vec3f(0, 0, bz, colors[!src].w);
                                            frameBuffer[(y * size.x + x) * 4] = finalColor.x * 255;
                                            frameBuffer[(y * size.x + x) * 4 + 1] = finalColor.y * 255;
                                            frameBuffer[(y * size.x + x) * 4 + 2] = finalColor.z * 255;
                                            frameBuffer[(y * size.x + x) * 4 + 3] = finalColor.w * 255;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            winFrameBuffer.create(size.x, size.y, &frameBuffer[0]);
            fbTexture.loadFromImage(winFrameBuffer);
        }
    }
}
