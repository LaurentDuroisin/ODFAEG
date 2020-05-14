#include "test.hpp"
int main(int argc, char* argv[]) {
    test app(sf::VideoMode(800,600),"test");
    return app.exec();
}
