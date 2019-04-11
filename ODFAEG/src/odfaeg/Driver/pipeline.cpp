#include "../../../include/odfaeg/Driver/pipeline.hpp"
namespace odfaeg {
    namespace driver {
        Pipeline::Pipeline() t(run) {
            pipelineStages[0] = std::make_unique<TransformVertices>();
            pipelineStages[1] = std::make_unique<DriverGeometry>();
            pipelineStages[2] = std::make_unique<Rasterizer>;
            pipelineStages[3] = std::make_unique<Blending>;
            finished.store(false);
        }
        void Pipeline::setVertexArrayPointer(void* pointer) {
            vertexArrayPointer = pointer;
        }
        void Pipeline::setColorArrayPointer(void* pointer) {
            colorArrayPointer = pointer;
        }
        void Pipeline::setTexcoordsArrayPointer(void* pointer) {
            texCoordsArrayPointer = pointer;
        }
        void Pipeline::setDataSize(unsigned int dataSize) {
            this->dataSize = dataSize;
        }
        void Pipeline::drawVertexArray(float* transfomMatrix, float* viewMatrix, float* projMatrix, float* textureMatrix, float* viewportMatrix, unsigned int first, unsigned int mode, unsigned int vertexCount) {
            VertexArray va;
            for (unsigned int i = first; i < vertexCount; i++) {
                Vector3f position (vertexArrayPointer[i * dataSize], vertexArrayPointer[i*dataSize + 1], vertexArrayPointer[i*dataSize + 2]);
                Color color (colorArrayPointer[i*dataSize], colorArrayPointer[i*dataSize+1], colorArrayPointer[i*dataSize+2], colorArrayPointer[i*dataSize+3]);
                Vector2f texCoords(texCoordsArrayPointer[i*dataSize], texCoordsArrayPointer[i*dataSize+1]);
                va.append(Vertex(position, color, texCoords));
            }
            pipelineStages[0]->setTransformMatrix(transformMatrix);
            pipelineStages[0]->setViewMatrix(viewMatrix);
            pipelineStages[0]->setProjMatrix(projMatrix);
            pipelineStages[0]->setTextureMatrix(textureMatrix);
            pipelineStages[0]->setViewportMatrix(viewportMatrix);
            vertexBlocks.push_back(va);
        }
        void Pipeline::run() {
            running.store(true);
            while (running) {
                if (finished.load()) {
                    pipelineStages[0]->load(vertexBlocks.back());
                    finished.store(false)
                }
                for (unsigned int i = 1; i < 4; i++) {
                    if (pipelineStages[i-1]->done()) {
                        pipelineStages[i]->load(pipelineStages[i-1]->getResults());
                    }
                }
                if (pipelineStages[4]->done()) {
                    finished.store(true);
                }
            }
        }
    }
}
