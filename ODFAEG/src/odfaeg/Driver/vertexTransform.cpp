#include "../../../include/odfaeg/Driver/vertexTransform.hpp"
#include <thread>
namespace odfaeg {
    namespace driver {
        VertexTransform::VertexTransform() : t(processVertices) {
            verticesTransformed.store(false);
        }
        void VertexTransform::setTransformMatrix(float* matrix) {
            transformMatrix.setFromArray(matrix);
        }
        void VertexTransform::setViewMatrix(float* matrix) {
            viewMatrix.setFromArray(matrix);
        }
        void VertexTransform::setProjMatrix(float* matrix) {
            projMatrix.setFromArray(matrix);
        }
        void VertexTransform::setViewportMatrix(float* matrix) {
            viewportMatrix.setFromArray(matrix);
        }
        void VertexTransform::setTextureMatrix(float* matrix) {
            textureMatrix.setFromArray(matrix);
        }
        void VertexTransform::load(any any) {
            addVertices(any.get<VertexArray>());
        }
        void VertexTransform::addVertices(VertexArray vertices) {
            this->vertices = vertices;
            verticesTransformed.store(false);
        }
        any VertexTransform::getResults() {
            verticesTransformed.store(false);
            return transformedVertices;
        }
        bool VertexTransform::done() {
            return verticesTransformed.load();
        }
        void VertexTransform::processVertices() {
            running.store(true);
            while(running.load()) {
                if (!verticesTransformed.load()) {
                    transformedVertices.clear();
                    for (unsigned int i = 0; i < vertices.getVertexCount(); i++) {
                        math::Vec3f position (vertices[i].position.x, vertices[i].position.y, vertices[i].position.z);
                        Color color = vertices[i].color;
                        math::Vec2f texCoords (vertices[i].texCoords.x, vertices[i].texCoords.y);
                        position = transformMatrix.transform(position);
                        position = viewMatrix.transform(position);
                        position = projMatrix.project(position);
                        position = position.normalizeToVec3();
                        position = viewportMatrix.toViewportCoordinates(position);
                        texCoords = textureMatrix * position;
                        transformedVertices.append(Vertex(position, color, texCoords));
                    }
                    vertices.clear();
                    verticesTransformed.store(true);
                }
            }
            t.join();
        }
        VertexTransform::~ṼertexTransform() {
            running.store(false);
        }
    }
}
