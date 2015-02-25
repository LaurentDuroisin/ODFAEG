#ifndef PONG_PARTIE_HPP
#define PONG_PARTIE_HPP
#include "player.hpp"
#include "ball.hpp"
class Partie {
public :
    Partie (Player& p1, Player& p2);
    void start();
    void finished();
    bool isRunning();
    unsigned int id();
    Player& player1();
    Player& player2();
private :
    unsigned int m_id;
    bool running;
    Ball m_ball;
    Player& m_player1;
    Player& m_player2;
    static unsigned int nbParties;
};
#endif // PONG_PARTIE_HPP
