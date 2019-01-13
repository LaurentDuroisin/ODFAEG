#include "application.hpp"
int main(int argc, char* argv[]) {
    ODFAEGCreator app(sf::VideoMode(1200,700),"ODFAEG Creator");
    return app.exec();
}
