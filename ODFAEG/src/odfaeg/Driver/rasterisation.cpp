#include "../../../include/odfaeg/Driver/rasterisation.hpp"
namespace odfaeg {
    namespace driver {
        Rasterizer::Rasterizer(Viewport viewport) t(processVertices), viewport(viewport) {
            rasterisationDone.store(false);
        }
        bool Rasterizer::done() {
            return rasterizationDone.load();
        }
        any Rasterizer::getResults() {
            rasterizationDone.store(false);
            return fragments;
        }
        void Rasterizer::emitFragments(VertexArray vertices) {
            this->vertices = vertices;
        }
        void Rasterizer::load(any any) {
            emitFragments(any.get<VertexArray>());
        }
        Rasterizer::processVertices() {
            running.load(true);
            while(running) {
                if (!rasterizationDone.load()) {
                    fragments.clear();
                    for (unsigned int i = 0; i < vertices.getVertexCount()-2; i++) {
                        Vertex v1 = vertices[i];
                        Vertex v2 = vertices[i+1];
                        Vertex v3 = vertices[i+2];
                        std::array<math::Vec3f, 3> verts = {math::Vec3f(v1.position.x, v1.position.y, v1.position.z),math::Vec3f(v2.position.x, v2.position.y, v2.position.z), math::Vec3f(v3.position.x, v3.position.y, v3.position.z)};
                        std::array<FragmentEdge, 3> edges = createFragmentEdges(verts);
                        interpolate(edges, verts);
                    }
                    vertices.clear();
                    rasterizationDone.store(true);
                }
            }
            t.join();
        }
        std::array<FragmentEdge, 3> Rasterizer::createFragmentEdges(std::array<Vertex, 3> verts) {
            std::array<FragmentEdge, 3> edges;
            std::array<std::array<float, 2>, 3> extends = math::Computer::getExtends(verts);
            int minX = extends[0][0];
            int minY = extends[1][0];
            int maxX = extends[0][1];
            int maxY = extends[1][1];
            math::Vec3f p1 = verts[0].position;
            math::Vec3f p2 = verts[1].position;
            math::Vec3f p3 = verts[2].position;
            int a01 = p1.y - p2.y, b01 = p2.x - p1.x;
            int a12 = p2.y - p3.y, b12 = p3.x - p2.x;
            int a20 = p3.y - p1.y, b20 = p1.x - p3.x;
            math::Vec2f p (minX, minY);
            math::Vec2f m (maxX, maxY);
            edges.push_back(FragmentEdge (verts[1], verts[2], p, m);
            edges.push_back(FragmentEdge (verts[2], verts[0], p, m));
            edges.push_back(FragmentEdge (verts[0], verts[1], p, m));
            return edges;
        }
        void Rasterizer::interpolate(std::array<FragmentEdge, 3> edges, std::array<Vertex, 3> verts) {
            math::Vec3f w0_row = edges[0].getRow();
            math::Vec3f w1_row = edges[1].getRow();
            math::Vec3f w2_row = edges[2].getRow();
            math::Vec2f p = edges[0].getMins();
            math::Vec2f m = edges[0].getMaxs();
            Vector3f p1 = verts[0].position;
            Vector3f p2 = verts[1].position;
            Vector3f p3 = verts[2].position;
            math::Vec3f w0_row = edges[0].getRaw();
            math::Vec3f w1_row = edges[1].getRaw();
            math::Vec3f w2_row = edges[2].getRaw();
            for (p.y = mins.y; p.y < maxs.y; p.y += FragmentEdge::stepYSize) {
                math::Vec3f w0 = w0_row;
                math::Vec3f w1 = w1_row;
                math::Vec3f w2 = w2_row;
                for (p.x = mins.x; p.x < maxs.x; p.x += FragmentEdge::stepXSize) {
                    math::Vec3f mask ((int) w0.x | (int) w1.x | (int) w2.x,
                                      (int) w0.y | (int) w1.y | (int) w2.y,
                                      (int) w0.z | (int) w1.z | (int) w2.z,
                                      (int) w0.w | (int) w1.w | (int) w2.w);
                    if (mask.x < 0 || mask.y < 0 || mask.z < 0 || mask.w < 0) {
                        for (unsigned int y = p.y; y < p.y + Edge::stepYSize; y++) {
                            for (unsigned int x = p.x; x < p.x + Edge::stepXSize; x++) {
                                math::Matrix2f m1(p1.x - p3.x, p2.x - p3.x,
                                                                  p1.y - p3.y, p2.y - p3.y);
                                float u = ((p2.y - p3.y) * (x - p3.x) + (p3.x - p2.x) * (y - p3.y)) / m1.getDet();
                                float v = ((p3.y - p1.y) * (x - p3.x) + (p1.x - p3.x) * (y - p3.y)) / m1.getDet();
                                float w = 1 - u - v;
                                math::Vec3f z (p1.z, p2.z, p3.z);
                                float bz = z.x * u + z.y * v + z.z * w;
                                if(x >= viewport.getPosition().x && y >= viewport.getPosition().y && z >= viewport.getPosition().z
                                && x < viewport.getPosition().x + viewport.getSize().x && y < viewport.getPosition().y + viewport.getSize().y && z < viewport.getSize().z) {
                                    math::Vec3f r = math::Vec3f (c1.r / 255.f, c2.r / 255.f, c3.r / 255.f);
                                    math::Vec3f g = math::Vec3f (c1.g / 255.f, c2.g / 255.f, c3.g / 255.f);
                                    math::Vec3f b = math::Vec3f (c1.b / 255.f, c2.b / 255.f, c3.b / 255.f);
                                    math::Vec3f a = math::Vec3f (c1.a / 255.f, c2.a / 255.f, c3.a / 255.f);
                                    math::Vec3f fragColor = math::Vec3f(r.x * u + r.y * v + r.z * w,
                                                                            g.x * u + g.y * v + g.z * w,
                                                                            b.x * u + b.y * v + b.z * w,
                                                                            a.x * u + a.y * v + a.z * w);
                                    Fragment frag(math::Vec3f(x, y, bz), Color(fragColor.r, fragColor.g, fragColor.b, fragColor.a));
                                    fragments.push_back(frag);
                                }
                            }
                        }
                    }
                }
            }
        }
        Rasterizer::~Rasterizer() {
            running.store(false);
        }
    }
}
