#include "pong.hpp"
using namespace odfaeg::network;
using namespace odfaeg::core;
using namespace odfaeg::graphic;
using namespace odfaeg::math;
void Pong::onInit() {
    Network::startSrv(10000, 10001, true);
    sf::Clock clock1, clock2;
    addClock(clock1, "Sync");
    addClock(clock2, "ElapsedTimeSinceLastSync");
    timeBtwnTwoSync = sf::seconds(1.f);
    Map* map = new Map(nullptr, "PONG_MAP", 50, 50);
    World::addEntityManager(map);
    World::setCurrentEntityManager("PONG_MAP");
}
void Pong::onExec() {
    std::vector<Player*>::iterator it;
    for (it = players.begin(); it != players.end();) {
        if (!(*it)->isConnected())
            it = players.erase(it);
        else
            it++;
    }
    std::vector<Partie*>::iterator it2;
    for (it2 = parties.begin(); it2 != parties.end(); it2++) {
        if (!(*it2)->player1().isConnected() || !(*it2)->player2().isConnected())
            it2 = parties.erase(it2);
        else
            it2++;
    }
    if (Network::hasRequest()) {
        User* user;
        std::string request = Network::getLastRequest(&user);
        std::vector<std::string> infos = split(request, "*");
        if (infos[0] == "SET_TIME") {
            for (unsigned int i = 0; i < players.size(); i++) {
                if (players[i]->ipAddress() == user->getIpAddress()) {
                    sf::Int64 cli_time = conversionStringLong(infos[1]);
                    sf::Int64 srv_time = getClock("TimeClock").getElapsedTime().asMicroseconds();
                    sf::Int64 syncTime = cli_time + (srv_time - players[i]->lastSrvTime()) * 0.5f;
                    players[i]->setSyncTime(syncTime);
                    players[i]->timeSinceLastSync().restart();
                }
            }
        }
        if (infos[0] == "CONNECT") {
            std::string pseudo = infos[1];
            Player* player = new Player(*user, pseudo, Vec2f(0, 0));
            players.push_back(player);
        }
    }
    if (getClock("Sync").getElapsedTime().asSeconds() >= timeBtwnTwoSync.asSeconds()
        && players.size() > 0) {
        SymEncPacket packet;
        std::string message = "GET_TIME";
        packet<<message;
        for (unsigned int i = 0; i < players.size(); i++) {
            sf::Int64 lastSrvTime = getClock("TimeClock").getElapsedTime().asMilliseconds();
            players[i]->setLastSrvTime(lastSrvTime);
        }
        Network::sendTcpPacket(packet);
        getClock("Sync").restart();
    }
}
