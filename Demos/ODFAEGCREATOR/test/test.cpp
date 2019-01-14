#include "/home/laurent/Développement/Projets-c++/ODFAEGCREATOR/test/test.hpp"
using namespace odfaeg::graphic;
test::test(sf::VideoMode vm, std::string title) : 
Application (vm, title, sf::Style::Resize|sf::Style::Close, sf::ContextSettings(0, 0, 0, 3, 0)) {
}
void test::onLoad() {
}
void test::onInit() {
}
void test::onRender(RenderComponentManager *cm) {
}
void test::onDisplay(RenderWindow* window) {
   if (&getRenderWindow() == window) {
       for (unsigned int i = 0; i < drawables.size(); i++) {
           window->draw(*drawable[i]);
       }
   }
}
void test::onUpdate (RenderWindow* window, sf::Event& event) {
 if (&getRenderWindow() == window && event.type == sf::Event::Closed) {
  stop();
 }
}
void test::onExec () {
}
