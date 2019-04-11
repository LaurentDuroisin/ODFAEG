#ifndef ODFAEG_GEOMETRY_HPP
#define ODFAEG_GEOMETRY_HPP
namespace odfaeg {
    namespace driver {
        class DriverGeometry : public PipelineStage {
            public :
            DriverGeometry ();
            void load(core::any any);
            void emitVertices(VertexArray va);
            void processVertices();
            core::any getResults();
            void setPointSize(unsigned int pointsize);
            bool done();
            private :
            template <std::size_t N>
            Vertex interpolate(std::array<Vertex, N> vertices) {
                math::Vec3f c;
                Vector3f p;
                Vector2f t;
                for (unsigned int i = 0; i < vertices.size(); i++) {
                    p += vertices[i];
                    c.x += vertices[i].color.r;
                    c.y += vertices[i].color.g;
                    c.z += vertices[i].color.b;
                    c.w += vertices[i].color.a;
                    t += vertices[i].texCoords;
                }
                p /= vertices.size();
                c /= vertices.size();
                t /= vertices.size();
                return Vertex(p, c, t);
            }
            ~DriverGeometry();
            VertexArray emittedVertices;
            VertexArray vertices;
            unsigned int pointSize;
            unsigned int lineWidth;
            bool geometryUpdated;
            std::thread t;
        };
    }
}
#endif // ODFAEG_GEOMETRY_HPP
