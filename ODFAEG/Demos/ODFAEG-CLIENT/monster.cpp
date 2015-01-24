#include "monster.h"
#include <iostream>
using namespace std;
using namespace odfaeg::core;
using namespace odfaeg::graphic;
using namespace odfaeg::physic;
using namespace odfaeg::math;
Monster::Monster (std::string name, std::string race, std::string currentMapName, int level, BoundingPolyhedron zone) : AnimatedEntity (Vec3f(-25, -50, 0), Vec3f (50, 100, 0), Vec3f(25, 50, 0), "E_MONSTER") {
    currentAnimIndex = 0;
    this->name = name;
    this->race = race;
    this->currentMapName = currentMapName;
    currentPointIndex = 0;
    speed = 0.0001f;
    moving = false;
    dir = Vec2f(0, 1);
    this->life = 100;
    this->maxLife = 100;
    range = 50;
    attackSpeed = 1.f;
    attack = 10;
    fightingMode = attacking;
    alive = true;
    regenHpSpeed = 1.f;
    regenHpAmount = 1;
    this->zone = zone;
}
void Monster::respawn() {
     std::array<std::array<float, 2>, 3> exts = Computer::getExtends(zone.getPoints());
     int x = Math::random(exts[0][1], exts[0][0]);
     int y = Math::random(exts[1][1], exts[1][0]);
     int z = y;
     setCenter(Vec3f(x, y, z));
}
BoundingPolyhedron& Monster::getZone() {
    return zone;
}
void Monster::onMove(Vec3f& t) {
    Entity::onMove(t);
    if (getCollisionVolume() != nullptr) {
        getCollisionVolume()->move(Vec3f(t.x, t.y, 0));
    }
}
float Monster::getRegenHpSpeed () {
    return regenHpSpeed;
}
void Monster::setRegenHpSpeed(float regenHpSpeed) {
    this->regenHpSpeed = regenHpSpeed;
}
sf::Time Monster::getTimeOfLastHpRegen() {
    return clockRegenHp.getElapsedTime();
}
void Monster::setLevel(int level) {
    this->level = level;
}
void Monster::setSpeed(float speed) {
    this->speed = speed;
}
float Monster::getSpeed() {
    return speed;
}
int Monster::getRegenHpAmount() {
    return regenHpAmount;
}
void Monster::setRegenHpAmount(int regenHpAmount) {
    this->regenHpAmount = regenHpAmount;
}
void Monster::setLife(int life) {
    this->life = life;
    clockRegenHp.restart();
}
int Monster::getLife() {
return life;
}
void Monster::setRange(int range) {
    this->range = range;
}
int Monster::getRange() {
    return range;
}
void Monster::setAttackSpeed (float attackSpeed) {
    this->attackSpeed = attackSpeed;
}
float Monster::getAttackSpeed () {
    return attackSpeed;
}
void Monster::setAttacking(bool b) {
    this->attacking = b;
}
void Monster::setAlive(bool b) {
    alive = b;
}
bool Monster::isAlive () {
    return alive;
}
bool Monster::isAttacking() {
    return attacking;
}
void Monster::setFightingMode(bool b) {
    this->fightingMode = b;
}
bool Monster::operator== (Entity &other) {
    if (getType() != other.getType())
        return false;
        Monster& monster = static_cast<Monster&>(other);
        if (anims.size() != monster.anims.size())
            return false;
        for (unsigned int i = 0; i < anims.size(); i++) {
            if (anims[i] != monster.anims[i])
                return false;
        }
    return true;
}
bool Monster::isInFightingMode() {
    return fightingMode;
}
void Monster::setAttack(int attack) {
    this->attack = attack;
}
int Monster::getAttack() {
    return attack;
}
sf::Time Monster::getTimeOfLastAttack() {
    return clockAtkSpeed.getElapsedTime();
}
void Monster::setDir (Vec2f dir) {
    if (isMoving())
        anims[currentAnimIndex]->stop();
    float angleRadians = dir.getAngleBetween(Vec2f::yAxis);
    int angle = Math::toDegrees(angleRadians);
    //Sud
    if (angle >= -10 && angle <= 10)
        currentAnimIndex = 4;
    //Sud ouest
    else if (angle > -80 && angle < -10)
        currentAnimIndex = 6;
    //Ouest
    else if (angle >= -100 && angle <= -80)
        currentAnimIndex = 7;
    //Nord ouest
    else if (angle > -170 && angle < -100)
        currentAnimIndex = 3;
    //Nors est
    else if (angle > 100 && angle < 170)
        currentAnimIndex = 2;
    //Est
    else if (angle >= 80 && angle <= 100)
        currentAnimIndex = 0;
    //Sud est
    else if (angle > 10 && angle < 80)
        currentAnimIndex = 5;
    //Nord
    else
        currentAnimIndex = 1;
    if (attacking)
        currentAnimIndex += 8;
    this->dir = dir;
    if (isMoving()) {
        anims[currentAnimIndex]->play(true);
    }
}
Vec2f Monster::getDir () {
    return dir;
}
void Monster::setMoving (bool b) {
    this->moving = b;
    if (moving) {
        anims[currentAnimIndex]->play(true);
    } else {
        anims[currentAnimIndex]->stop();
        anims[currentAnimIndex]->setCurrentTile(0);
    }
}
bool Monster::isMoving () {
    return moving;
}
void Monster::setPath(vector<Vec2f> path) {
    this->path = path;
}
vector<Vec2f> Monster::getPath() {
    return path;
}
void Monster::addAnimation (Anim *anim) {
    addChild(anim);
    anim->setParent(this);
    anims.push_back(anim);
}
Anim* Monster::getAnimation(unsigned int index) {
    if (index >= 0 && index < anims.size())
        return anims[index];
    return NULL;
}
unsigned int Monster::getCurrentPathIndex() {
    return currentPointIndex;
}
void Monster::setCurrentPathIndex (unsigned int currentPointIndex) {
    this->currentPointIndex = currentPointIndex;
}
void Monster::setMaxLife(int life) {
    this->maxLife = maxLife;
}
int Monster::getMaxLife() {
    return maxLife;
}
int Monster::getLevel() {
    return level;
}
string Monster::getRace () {
    return race;
}
void Monster::onDraw(RenderTarget &target, RenderStates states) const {
    target.draw(*getCurrentEntity(), states);
}
Entity* Monster::getCurrentEntity() const {
    return anims[currentAnimIndex]->getCurrentEntity();
}
Monster::~Monster() {
}






