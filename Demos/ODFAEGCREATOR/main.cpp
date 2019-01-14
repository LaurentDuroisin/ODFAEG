#include "application.hpp"
using namespace odfaeg::graphic::gui;
using namespace odfaeg::graphic;
using namespace odfaeg::physic;
using namespace odfaeg::math;
int main(int argc, char* argv[]) {
    EXPORT_CLASS_GUID(BoundingVolumeBoundingBox, BoundingVolume, BoundingBox)
    EXPORT_CLASS_GUID(ShapeRectShape, Shape, RectangleShape)
    ODFAEGCreator app(sf::VideoMode(1200,700),"ODFAEG Creator");
    return app.exec();
}
