#include "application.h"
#include "odfaeg/Core/clock.h"
#include "odfaeg/Graphics/GUI/label.hpp"
using namespace odfaeg::core;
using namespace odfaeg::math;
using namespace odfaeg::physic;
using namespace odfaeg::graphic;
using namespace odfaeg::audio;
using namespace sorrok;
using namespace std;
int main(int argc, char* argv[])
{
    EXPORT_CLASS_GUID(BoundingVolumeBoundingBox, BoundingVolume, BoundingBox)
    EXPORT_CLASS_GUID(EntityTile, Entity, Tile)
    EXPORT_CLASS_GUID(EntityTile, Entity, BigTile)
    EXPORT_CLASS_GUID(EntityWall, Entity, g2d::Wall)
    EXPORT_CLASS_GUID(EntityDecor, Entity, g2d::Decor)
    EXPORT_CLASS_GUID(EntityAnimation, Entity, Anim)
    EXPORT_CLASS_GUID(EntityHero, Entity, Hero)
    MyAppli app(sf::VideoMode(800, 600, 32), "Test odfaeg");
    return app.exec();
    // Create the main window.
    RenderWindow window(sf::VideoMode(800, 600), "test");
    View currentView(600, 800, 0, 600);
    currentView.move(300, 300, 300);
    Vec3f point(0, 0, 0);
    Vec3f fragCoord = window.mapCoordsToPixel(point, currentView);
    View previousView(600, 800, 0, 600);
    Vec3f pointToHave = window.mapCoordsToPixel(point, previousView);
    std::cout<<currentView.getViewVolume().getPosition()<<"frag coord : "<<fragCoord<<" point to have : "<<pointToHave<<std::endl;
    fragCoord = window.mapPixelToCoords(fragCoord, currentView);
    fragCoord = window.mapCoordsToPixel(fragCoord, previousView);
    std::cout<<"coords : "<<fragCoord<<std::endl;
    return 0;
}




