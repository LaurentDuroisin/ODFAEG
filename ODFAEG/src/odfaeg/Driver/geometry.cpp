#include "../../../include/odfaeg/Driver/geometry.hpp"
namespace odfaeg {
    namespace driver {
        using namespace sf;
        DriverGeometry::DriverGeometry() : t(processVertices) {
            geometryUpdated.store(false);
        }
        void DriverGeometry::processVertices() {
            running.store(true);
            while(running.load()) {
                if (!geometryUpdated.load()) {
                     emmittedVertices.clear();
                     if (primitiveType == GL_POINTS) {
                        for (unsigned int i = 0; i < vertices.size(); i++) {
                            float angle = math::Math::toRadians(360) / quality;
                            for (unsigned int i = 0; i < 16; i++) {
                                Vector3f p1(vertices[i].x, vertices[i].y, vertices[i].z);
                                Vector3f p2(vertices[i].x + pointSize * 0.5f * math::Math::cosinus (i * angle), vertices[i].y  + math::Math::cosinus (i * angle), vertices[i].z + math::Math::cosinus (i * angle));
                                Vector3f p3(vertices[i].x + pointSize * 0.5f * math::Math::cosinus ((i + 1) * angle), vertices[i].y  + math::Math::cosinus ((i + 1) * angle), vertices[i].z + math::Math::cosinus ((i + 1) * angle));
                                Vertex v2(p2, vertices[i].color, vertices[i].texCoords);
                                Vertex v3(p3, vertices[i].color, vertices[i].texCoords);
                                Vertex v1 (p1, vertices[i].color, vertices[i].texCoords);
                                emittedVertices.append(v1);
                                emittedVertices.append(v2);
                                emittedVertices.append(v3);
                            }
                        }
                    } else if (primitiveType == GL_LINES) {
                        for (unsigned int i = 0; i < vertices.size() / 2; i+=2) {
                            Vector3f p1 (vertices[i].x + lineWidth * 0.5f, vertices[i].y + lineWidth * 0.5f, vertices[i].z + lineWidth * 0.5f);
                            Vector3f p2 (vertices[i].x - lineWidth * 0.5f, vertices[i].y - lineWidth * 0.5f, vertices[i].z - lineWidth * 0.5f);
                            Vector3f p3 (vertices[i+1].x + lineWidth * 0.5f, vertices[i+1].y + lineWidth * 0.5f, vertices[i+1].z + lineWidth * 0.5f);
                            Vector3f p4 (vertices[i+1].x - lineWidth * 0.5f, vertices[i+1].y - lineWidth * 0.5f, vertices[i+1].z - lineWidth * 0.5f);
                            Vertex v1 (p1, vertices[i].color, vertices[i].texCoords);
                            Vertex v2 (p2, vertices[i].color, vertices[i].textCoords);
                            Vertex v3 (p3, vertices[i+1].color, vertices[i+1].texCoords);
                            Vertex v4 (p4, vertices[i+1].color, vertices[i+1].textCoords);
                            std::array<Vertex, 4> vertices = {v1, v2, v3, v4};
                            Vertex c = interpolate(vertices);
                            emittedVertices.append(c);
                            emittedVertices.append(v1);
                            emittedVertices.append(v2);
                            emittedVertices.append(c);
                            emittedVertices.append(v3);
                            emittedVertices.append(v4);
                        }
                    } else if (primitiveType == GL_LINE_STRIP) {
                        for (unsigned int i = 0; i < vertices.size()-1; i++) {
                            Vector3f p1 (vertices[i].x + lineWidth * 0.5f, vertices[i].y + lineWidth * 0.5f, vertices[i-1].z + lineWidth * 0.5f);
                            Vector3f p2 (vertices[i].x - lineWidth * 0.5f, vertices[i].y - lineWidth * 0.5f, vertices[i-1].z - lineWidth * 0.5f);
                            Vector3f p3 (vertices[i+1].x + lineWidth * 0.5f, vertices[i+1].y + lineWidth * 0.5f, vertices[i].z + lineWidth * 0.5f);
                            Vector3f p4 (vertices[i+1].x - lineWidth * 0.5f, vertices[i+1].y - lineWidth * 0.5f, vertices[i].z - lineWidth * 0.5f);
                            Vertex v1(p1, vertices[i].color, vertices[i].texCoords);
                            Vertex v2(p2, vertices[i].color, vertices[i].textCoords);
                            Vertex v3(p3, vertices[i+1].color, vertices[i+1].texCoords);
                            Vertex v4(p4, vertices[i+1].color, vertices[i+1].textCoords);
                            std::array<Vertex, 4> vertices = {v1, v2, v3, v4}
                            Vertex c (center, avgColor, avgTexCoords);
                            emittedVertices.append(c);
                            emittedVertices.append(v1);
                            emittedVertices.append(v2);
                            emittedVertices.append(c);
                            emittedVertices.append(v3);
                            emittedVertices.append(v4);
                        }
                    } else if (primitiveType == GL_LINE_LOOP) {
                        for (unsigned int i = 0; i < vertices.size(); i++) {
                           Vertex v1, v2, v3, v4;
                           if (i < vertices.size() - 1) {
                                Vector3f p1 (vertices[i].x + lineWidth * 0.5f, vertices[i].y + lineWidth * 0.5f, vertices[i].z + lineWidth * 0.5f);
                                Vector3f p2 (vertices[i].x - lineWidth * 0.5f, vertices[i].y - lineWidth * 0.5f, vertices[i].z - lineWidth * 0.5f);
                                Vector3f p3 (vertices[i+1].x + lineWidth * 0.5f, vertices[i+1].y + lineWidth * 0.5f, vertices[i+1].z + lineWidth * 0.5f);
                                Vector3f p4 (vertices[i+1].x - lineWidth * 0.5f, vertices[i+1].y - lineWidth * 0.5f, vertices[i+1].z - lineWidth * 0.5f);
                                v1 = Vertex(p1, vertices[i].color, vertices[i].texCoords);
                                v2 = Vertex(p2, vertices[i].color, vertices[i].textCoords);
                                v3 = Vertex(p3, vertices[i+1].color, vertices[i+1].texCoords);
                                v4 = Vertex(p4, vertices[i+1].color, vertices[i+1].textCoords);
                            } else {
                                Vector3f p1 (vertices[i].x + lineWidth * 0.5f, vertices[i].y + lineWidth * 0.5f, vertices[i].z + lineWidth * 0.5f);
                                Vector3f p2 (vertices[i].x - lineWidth * 0.5f, vertices[i].y - lineWidth * 0.5f, vertices[i].z - lineWidth * 0.5f);
                                Vector3f p3 (vertices[0].x + lineWidth * 0.5f, vertices[0].y + lineWidth * 0.5f, vertices[0].z + lineWidth * 0.5f);
                                Vector3f p4 (vertices[0].x - lineWidth * 0.5f, vertices[0].y - lineWidth * 0.5f, vertices[0].z - lineWidth * 0.5f);
                                v1 = Vertex(p1, vertices[i].color, vertices[i].texCoords);
                                v2 = Vertex(p2, vertices[i].color, vertices[i].textCoords);
                                v3 = Vertex(p3, vertices[0].color, vertices[0].texCoords);
                                v4 = Vertex(p4, vertices[0].color, vertices[0].textCoords);
                            }
                            std::array<Vertex, 4> vertices = {v1, v2, v3, v4};
                            Vertex c = interpolate(vertices);
                            emittedVertices.append(c);
                            emittedVertices.append(v1);
                            emittedVertices.append(v2);
                            emittedVertices.append(c);
                            emittedVertices.append(v3);
                            emittedVertices.append(v4);
                        }
                    } else if (primitiveType == GL_TRIANGLES) {
                        for (unsigned int i = 0; i < vertices.size() / 3; i+=3) {

                            Vector3f p1 (vertices[i].x, vertices[i].y, vertices[i].z);
                            Vector3f p2 (vertices[i+1].x, vertices[i+1].y, vertices[i+1].z);
                            Vector3f p3 (vertices[i+2].x, vertices[i+2].y, vertices[i+2].z);
                            Vertex v1 (p1, vertices[i].color, vertices[i].texCoords);
                            Vertex v2 (p2, vertices[i+1].color, vertices[i+1].texCoords);
                            Vertex v3 (p3, vertices[i+2].color, vertices[i+2].texCoords);
                            std::array<Vertex, 3> vertices = {v1, v2, v3};
                            Vertex c = interpolate(vertices);
                            emittedVertices.append(c);
                            emittedVertices.append(v1);
                            emittedVertices.append(v2);
                            emittedVertices.append(c);
                            emittedVertices.append(v2);
                            emittedVertices.append(v3);
                        }
                    } else if (primitiveType == GL_TRIANGLE_STRIP) {
                        for (unsigned int i = 0; i < vertices.size()-2; i++) {
                            Vector3f p1 (vertices[i].x, vertices[i].y, vertices[i].z);
                            Vector3f p2 (vertices[i+1].x, vertices[i+1].y, vertices[i+1].z);
                            Vector3f p3 (vertices[i+2].x, vertices[i+2].y, vertices[i+2].z);
                            Vertex v1(p1, vertices[i].color, vertices[i].texCoords);
                            Vertex v2(p2, vertices[i+1].color, vertices[i+1].texCoords);
                            Vertex v3(p3, vertices[i+2].color, vertices[i+2].texCoords);
                            std::array<Vertex, 3> vertices;
                            Vertex c = interpolate(vertices);
                            emittedVertices.append(c);
                            emittedVertices.append(v1);
                            emittedVertices.append(v2);
                            emittedVertices.append(c);
                            emittedVertices.append(v2);
                            emittedVertices.append(v3);
                        }
                    } else if (primitiveType == GL_TRIANGLE_FAN) {
                        for (unsigned int i = 0; i < vertices.size()-2; i++) {
                            Vector3f p1 (vertices[0].x, vertices[0].y, vertices[0].z);
                            Vector3f p2 (vertices[i+1].x, vertices[i+1].y, vertices[i+1].z);
                            Vector3f p3 (vertices[i+2].x, vertices[i+2].y, vertices[i+2].z);
                            Vertex v1(p1, vertices[0].color, vertices[0].texCoords);
                            Vertex v2(p2, vertices[i+1].color, vertices[i+1].texCoords);
                            Vertex v3(p3, vertices[i+2].color, vertices[i+2].texCoords);
                            std::array<Vertex, 3> vertices;
                            Vertex c = interpolate(vertices);
                            emittedVertices.append(c);
                            emittedVertices.append(v1);
                            emittedVertices.append(v2);
                            emittedVertices.append(c);
                            emittedVertices.append(v2);
                            emittedVertices.append(v3);
                        }
                    } else if (primitiveType == GL_QUADS) {
                        for (unsigned int i = 0; i < vertices.size() / 4; i+=4) {
                            Vector3f p1 (vertices[i].x, vertices[i].y, vertices[i].z);
                            Vector3f p2 (vertices[i+1].x, vertices[i+1].y, vertices[i+1].z);
                            Vector3f p3 (vertices[i+2].x, vertices[i+2].y, vertices[i+2].z);
                            Vector3f p4 (vertices[i+3].x, vertices[i+3].y, vertices[i+3].z);
                            Vertex v1(p1, vertices[i].color, vertices[i].texCoords);
                            Vertex v2(p2, vertices[i+1].color, vertices[i+1].texCoords);
                            Vertex v3(p3, vertices[i+2].color, vertices[i+2].texCoords);
                            Vertex v4(p4, vertices[i+3].color, vertices[i+3].texCoords);
                            std::array<Vertex, 4> vertices = {v1, v2, v3, v4};
                            Vertex c = interpolate(vertices);
                            emittedVertices.append(c);
                            emittedVertices.append(v1);
                            emittedVertices.append(v2);
                            emittedVertices.append(c);
                            emittedVertices.append(v2);
                            emittedVertices.append(v3);
                        }
                    }
                    vertices.clear();
                    geometryUpdated.store(true);
                }
            }
        }
        void DriverGeometry::load(core::any any) {
            emitVertices(any.get<VertexArrray>());
        }
        void DriverGeometry::emitVertices(VertexArray va) {
            vertices = va;
            primitiveType = va.getPrimitiveType();
            geometryUpdated.store(false);
        }
        void DriverGeometry::setPointSize(unsigned int pointSize) {
            this->pointSize = pointsize;
        }
        void DriverGeometry::setLineWidth(unsigned int lineWidth) {
            this->lineWidth = linewidth;
        }
        any DriverGeometry::getResults() {
            geometryUpdated.store(false);
            return emittedVertices;
        }
        bool DriverGeometry::done() {
            return geometryUpdated.load();
        }
        DriverGeometry::~DriverGeometry() {
            running.store(false);
        }
    }
}
