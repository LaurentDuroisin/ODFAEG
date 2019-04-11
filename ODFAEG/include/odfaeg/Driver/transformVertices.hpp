#ifndef ODFAEG_TRANSFORMVERTICES_HPP
#define ODFAEG_TRANSFORMVERTICES_HPP
#include <atomic>
namespace odfaeg {
    namespace driver {
        class TransformVertices : public PipelineStage {
            public :
                TransformVertices();
                void setVertexPointer(unsigned int stride);
                void setColorPointer(unsigned int stride);
                void setTexcoordsPointer(unsigned int stride);
                void setDataSize(unsigned int size);
                void setTransformMatrix(float* matrix);
                void setViewMatrix(float* matrix);
                void setProjMatrix(float* matrix);
                void setViewportMatrix(float* matrix);
                void setTextureMatrix(float* matrix);
                void load(core::any any);
                void addVertices(VertexArray va);
                bool done();
                core::any getTransformedVertices();
            private :
                TransformMatrix transformMatrix;
                ViewportMatrix viewportMatrix;
                ProjMatrix projMatrix;
                Matrix4f textureMatrix;
                float* vertices;
                VertexArray transformedVertices;
                std::atomic<bool> verticesTransformed;
                std::atomic<bool> running;
        };
    }
}
#endif
