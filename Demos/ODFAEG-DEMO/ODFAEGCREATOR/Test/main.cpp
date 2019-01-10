#include "test.hpp"
int main(int argc, char* argv[]) {
    Test app(sf::VideoMode(800,600),"Test");
    return app.exec();
}
