#include "application.h"
using namespace odfaeg::math;
using namespace odfaeg::physic;
using namespace odfaeg::core;
using namespace odfaeg::graphic;
using namespace odfaeg::window;
int main (int argv, char* argc[]) {
    MyAppli appli(Vec2f(800, 600), "test");
    return appli.exec();
}
