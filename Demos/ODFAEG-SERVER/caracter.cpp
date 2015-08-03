#include "caracter.h"
#include <iostream>
using namespace std;
using namespace odfaeg::core;
using namespace odfaeg::math;
using namespace odfaeg::graphic;
Caracter::Caracter (std::string type, string name, string currentMapName, string classs, int level)
: AnimatedEntity (Vec3f(-25, -50, 0), Vec3f (50, 100, 0), Vec3f(25, 50, 0), type) {
    currentAnimIndex = 0;
    this->classs = classs;
    this->level = level;
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
}
void Caracter::onMove(Vec3f& t) {
    Entity::onMove(t);
    if (getCollisionVolume() != nullptr) {
        getCollisionVolume()->move(Vec3f(t.x, t.y, 0));
    }
}
float Caracter::getRegenHpSpeed () {
    return regenHpSpeed;
}
void Caracter::setRegenHpSpeed(float regenHpSpeed) {
    this->regenHpSpeed = regenHpSpeed;
}
sf::Time Caracter::getTimeOfLastHpRegen() {
    return clockRegenHp.getElapsedTime();
}
void Caracter::setLevel(int level) {
    this->level = level;
}
void Caracter::setSpeed(float speed) {
    this->speed = speed;
}
float Caracter::getSpeed() {
    return speed;
}
int Caracter::getRegenHpAmount() {
    return regenHpAmount;
}
void Caracter::setRegenHpAmount(int regenHpAmount) {
    this->regenHpAmount = regenHpAmount;
}
void Caracter::setLife(int life) {
    this->life = life;
    clockRegenHp.restart();
}
int Caracter::getLife() {
return life;
}
void Caracter::setRange(int range) {
    this->range = range;
}
int Caracter::getRange() {
    return range;
}
void Caracter::setAttackSpeed (float attackSpeed) {
    this->attackSpeed = attackSpeed;
}
float Caracter::getAttackSpeed () {
    return attackSpeed;
}
void Caracter::setAttacking(bool b) {
    this->attacking = b;
}
void Caracter::setAlive(bool b) {
    alive = b;
}
bool Caracter::isAlive () {
    return alive;
}
bool Caracter::isAttacking() {
    return attacking;
}
void Caracter::setFightingMode(bool b) {
    this->fightingMode = b;
}
bool Caracter::operator== (Entity &other) {
    if (getType() != other.getType())
        return false;
        Caracter& caracter = static_cast<Caracter&>(other);
        if (anims.size() != caracter.anims.size())
            return false;
        for (unsigned int i = 0; i < anims.size(); i++) {
            if (anims[i] != caracter.anims[i])
                return false;
        }
    return true;
}
bool Caracter::isInFightingMode() {
    return fightingMode;
}
void Caracter::setAttack(int attack) {
    this->attack = attack;
}
int Caracter::getAttack() {
    return attack;
}
sf::Time Caracter::getTimeOfLastAttack() {
    return clockAtkSpeed.getElapsedTime();
}
void Caracter::setDir (Vec2f dir) {
    this->dir = dir;
}
Vec2f Caracter::getDir () {
    return dir;
}
void Caracter::setMoving (bool b) {
    this->moving = b;
}
bool Caracter::isMoving () {
    return moving;
}
void Caracter::setPath(vector<Vec2f> path) {
    this->path = path;
}
vector<Vec2f> Caracter::getPath() {
    return path;
}
void Caracter::addAnimation (Anim *anim) {
    addChild(anim);
    anim->setParent(this);
    anims.push_back(anim);
}
Anim* Caracter::getAnimation(unsigned int index) {
    if (index >= 0 && index < anims.size())
        return anims[index];
    return NULL;
}
void Caracter::setMaxLife(int life) {
    this->maxLife = maxLife;
}
int Caracter::getMaxLife() {
    return maxLife;
}
int Caracter::getLevel() {
    return level;
}
string Caracter::getClass () {
    return classs;
}
void Caracter::onDraw(RenderTarget &target, RenderStates states) {
    target.draw(*getCurrentEntity(), states);
}
Entity* Caracter::getCurrentEntity() const {
    return anims[currentAnimIndex]->getCurrentEntity();
}
Caracter::~Caracter() {
}




