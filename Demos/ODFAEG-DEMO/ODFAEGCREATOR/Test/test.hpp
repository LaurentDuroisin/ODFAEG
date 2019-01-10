#ifndef TEST_HPP
#define TEST_HPP
#include "/usr/local/include/odfaeg/Core/application.h"
class Test : public odfaeg::core::Application {
   public : 
   Test(sf::VideoMode vm, std::string title);
   void onLoad();
   void onInit();
   void onRender(odfaeg::graphic::RenderComponentManager* cm);
   void onDisplay(odfaeg::graphic::RenderWindow* window);
   void onUpdate (odfaeg::graphic::RenderWindow*, sf::Event& event);
   void onExec ();
};
#endif
