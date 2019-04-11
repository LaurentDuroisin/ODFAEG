#ifndef ODFAEG_FRAGMENTS_HPP
#define ODFAEG_FRAGMENTS_HPP
namespace odfaeg {
    namespace driver {
        class FragmentEdge {
        public :
            FragmentEdge (sf::Vertex v0, sf::Vertex v1, math::Vec2f p);
            math::Vec3f getRaw();
            math::Vec3f getOneStepX();
            math::Vec3f getOneStepY();
            math::Vec2f getMins();
            math::Vec2f getMaxs();
            const unsigned int stepXSize = 4;
            const unsigned int stepYSize = 4;
        private :
            math::Vec3f raw, oneStepX, oneStepY;
            math::Vec2f mins, maxs;
        };
        class Fragment {
            public :
                Fragment(math::Vec3f position, sf::Color color);
                sf::Color getColor();
                math::Vec3f getPosition();
            private :
                sf::Color color;
        };
    }
}
#endif // ODFAEG_FRAGMENTS_HPP
