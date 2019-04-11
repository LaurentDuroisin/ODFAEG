#ifndef ODFAEG_PIPELINE_HPP
#define ODFAEG_PIPELINE_HPP
namespace odfaeg {
    namespace driver {
        class Pipeline {
            public :
                Pipeline();
                void setVertexArrayPointer(void* pointer);
                void setColorArrayPointer(void* pointer);
                void setTexcoordsArrayPointer(void* pointer);
                void setDataSize(unsigned int dataSize);
                void drawVertexArray(float* transfomMatrix, float* viewMatrix, float* projMatrix, float* textureMatrix, float* viewportMatrix, unsigned int first, float* vertexArray, unsigned int vertexCount);
                void run();
            private :
                std::thread t;
                std::vector<VertexArray> vertexBlocks;
                std::array<std::unique_ptr<PipelineStage>, 5> pipelineStages;
                std::atomic<bool> finished, running;
                void* vertexArrayPointer, colorArrayPointer, texCoordsArrayPointer;
                unsigned int dataSize;
        };
    }
}
#endif
