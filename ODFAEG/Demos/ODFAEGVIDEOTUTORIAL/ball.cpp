#include "ball.hpp"
using namespace odfaeg::physic;
using namespace odfaeg::math;
Ball::Ball() : Entity (Vec2f(390, 290), Vec2f(20, 20),Vec2f(10, 10),"E_BALL") {
    m_dir = Vec2f(1, 1);
    m_dir = m_dir.normalize();
    BoundingVolume* ballArea = new BoundingSphere(Vec3f(400, 300, 0), 10);
    setCollisionVolume(ballArea);
    m_speed = 0.f;
}
void Ball::updateSpeed(float speed) {
    m_speed = speed;
}
void Ball::updateDir(Vec2f dir) {
    m_dir = dir;
}
float Ball::speed() {
    return m_speed;
}
Vec2f Ball::dir() {
    return m_dir;
}
void Ball::onMove(Vec3f& t) {
    getCollisionVolume()->move(t);
}
