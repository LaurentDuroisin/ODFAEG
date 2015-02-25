#ifndef PONG_HPP
#define PONG_HPP
#include "odfaeg/Core/application.h"
#include "odfaeg/Network/network.h"
#include "partie.hpp"
#include "odfaeg/Graphics/world.h"
#include "odfaeg/Graphics/map.h"
class Pong : public odfaeg::core::Application {
public :
    void onInit();
    void onExec();
private :
    sf::Time timeBtwnTwoSync;
    std::vector<Partie*> parties;
    std::vector<Player*> players;
};
#endif // PONG_HPP
