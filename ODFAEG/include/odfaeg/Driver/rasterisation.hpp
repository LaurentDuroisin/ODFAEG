#ifndef ODFAEG_RASTERISATION_HPP
#define ODFAEG_RASTERISATION_HPP
namespace odfaeg {
    namespace driver {
        class Rasterizer : public PipelineStage {
        public :
            Rasterizer(Viewport viewport);
            void load(core::any any);
            void emitFragments(std::vector<Vertex> vertices);
            void processVertices();
            bool done();
            core::any getResults();
            ~Rasterizer();
        private :
            std::array<FragmentEdge, 3> createFragmentEdges(std::array<Vertex, 3> verts);
            void interpolate(std::array<FragmentEdge, 3> edges, verts);
            std::atomic<bool> rasterizationDone;
            std::thread t;
            VertexArray vertices;
            std::vector<Fragment> fragments;
            std::atomic<bool> running;
        };
    }
}
#endif // ODFAEG_RASTERISATION_HPP

