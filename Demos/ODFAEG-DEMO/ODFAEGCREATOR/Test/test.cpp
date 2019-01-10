#include "/home/laurent/Développement/Projets-c++/ODFAEGCREATOR/Test/test.hpp"
using namespace odfaeg::graphic;
Test::Test(sf::VideoMode vm, std::string title) : 
Application (vm, title, sf::Style::Resize|sf::Style::Close, sf::ContextSettings(0, 0, 0, 3, 0)) {
}
void Test::onLoad() {
}
void Test::onInit() {
}
void Test::onRender(RenderComponentManager *cm) {
}
void Test::onDisplay(RenderWindow* window) {
}
void Test::onUpdate (RenderWindow* window, sf::Event& event) {
 if (&getRenderWindow() == window && event.type == sf::Event::Closed) {
  stop();
 }
}
void Test::onExec () {
}
