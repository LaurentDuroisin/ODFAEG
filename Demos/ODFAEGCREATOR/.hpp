#ifndef �,n_HPP
#define �,n_HPP
#include odfaeg/core/application.hpp
class  : public odfaeg::core::Application {
 (sf::VideoMode vm, std::string title);
 void onLoad();
 void onInit();
 void onRender(odfaeg::graphic::RenderComponentManager* cm);
 void onDisplay(odfaeg::graphic::RenderWindow* window);
 void onUpdate (odfaeg::graphic::RenderWindow*, sf::Event& event);
 void onExec ();
};
#endif
