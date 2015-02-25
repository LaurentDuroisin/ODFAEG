#include "player.hpp"
using namespace odfaeg::network;
using namespace odfaeg::physic;
using namespace odfaeg::math;
Player::Player(User& user, std::string pseudo, Vec2f position) :
    Entity (position, Vec2f(10, 50), Vec2f(5, 25), "E_PLAYER"),
    m_user(user),
    m_pseudo(pseudo) {
    BoundingVolume* racketArea = new BoundingBox(position.x, position.y, 0, 10, 50, 0);
    setCollisionVolume(racketArea);
}
bool Player::isConnected() {
    return &m_user != nullptr;
}
sf::Clock& Player::timeSinceLastSync() {
    return m_timeSinceLastSync;
}
sf::IpAddress Player::ipAddress() {
    return m_user.getIpAddress();
}
std::string Player::pseudo() {
    return m_pseudo;
}
void Player::setMoving(bool moving) {
    this->moving = moving;
}
bool Player::isMoving() {
    return moving;
}
void Player::onMove(Vec3f& t) {
    getCollisionVolume()->move(t);
}
void Player::setDir(Vec2f dir) {
    m_dir = dir;
}
Vec2f Player::dir() {
    return m_dir;
}
void Player::setLastSrvTime(sf::Int64 lastSrvTime) {
    this->m_lastSrvTime = lastSrvTime;
}
void Player::setSyncTime(sf::Int64 syncTime) {
    this->m_syncTime = syncTime;
}
sf::Int64 Player::lastSrvTime() {
    return m_lastSrvTime;
}
sf::Int64 Player::syncTime() {
    return m_syncTime;
}
