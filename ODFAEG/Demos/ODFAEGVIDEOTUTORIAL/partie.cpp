#include "partie.hpp"
unsigned int Partie::nbParties = 0;
Partie::Partie(Player& p1, Player& p2) :
    m_player1(p1), m_player2(p2) {
    m_id = nbParties;
    nbParties++;
}
void Partie::start() {
    running = true;
}
void Partie::finished() {
    running = false;
}
bool Partie::isRunning() {
    return running;
}
unsigned int Partie::id() {
    return m_id;
}
Player& Partie::player1() {
    return m_player1;
}
Player& Partie::player2() {
    return m_player2;
}
