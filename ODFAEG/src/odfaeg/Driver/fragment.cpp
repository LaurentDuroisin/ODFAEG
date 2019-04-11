namespace odfaeg {
    namespace driver {
        FragmentEdge::FragmentEdge(Vertex v0, Vertex v1, math::Vec2f p, math::Vec2f m) {
            //Fragment setup
            int a = v0.y - v1.y, b = v1.x - v0.x;
            int c = v0.x*v1.y - v0.y*v1.x;

            // Step deltas
            oneStepX = math::Vec3f(a * stepXSize, a * stepXSize, a * stepXSize, a * stepXSize);
            oneStepY = math::Vec3f(b * stepYSize, b * stepYSize, b * stepYSize, b * stepYSize);

            // x/y values for initial pixel block
            math::Vec3f x = math::Vec3f(origin.x, origin.x, origin.x, origin.x) + math::Vec3f(0,1,2,3);
            math::Vec3f y = math::Vec3f(origin.y, origin.y, origin.y, origin.y) + math::Vec3f(0,1,2,3);

            // Fragment function values at origin
            raw = math::Vec3f(a, a, a, a)*x + math::Vec3f(b, b, b, b)*y + math::Vec3f(c, c, c, c);
            mins = p;
            maxs = m;
        }
        math::Vec2f FragmentEdge::getMins() {
            return mins;
        }
        math::Vec2f FragmentEdge::getMaxs() {
            return maxs;
        }
        math::Vec3f FragmentEdge::getRaw() {
            return raw;
        }
        math::Vec3f FragmentEdge::getOneStepX() {
            return oneStepX;
        }
        math::Vec3f FragmentEdge::getOneStepY() {
            return oneStepY;
        }
        Fragment::Fragment(Vec3f position, Color color) : position(position), color(color) {

        }
        Fragment::getColor() {
            return color;
        }
        Vec3f Fragment::getPosition() {
            return position;
        }
    }
}
