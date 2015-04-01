#include "caracter.h"
#include <iostream>
using namespace std;
using namespace odfaeg::math;
using namespace odfaeg::graphic;
Caracter::Caracter (string factionName, string pseudo, string sex, string currentMapName, string hairColor,
    string eyesColor, string skinColor, string faceType, string classs, int level) : AnimatedEntity (Vec3f(-25, -50, 0), Vec3f (50, 100, 0), Vec3f(25, 50, 0), "E_CARACTER") {
    currentAnimIndex = 0;
    this->factionName = factionName;
    this->pseudo = pseudo;
    this->sex = sex;
    this->currentMapName = currentMapName;
    this->hairColor = hairColor;
    this->eyesColor = eyesColor;
    this->faceType = faceType;
    this->skinColor = skinColor;
    this->classs = classs;
    this->level = level;
    currentPointIndex = 0;
    speed = 0.0001f;
    moving = false;
    dir = Vec2f(0, 1);
    this->life = 100;
    this->maxLife = 100;
    range = 50;
    attackSpeed = 1.f;
    attack = 10;
    fightingMode = attacking = moveFromKeyboard = false;
    alive = true;
    xp = 0;
    xpReqForNextLevel = 1500;
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
void Caracter::setCurrentXp(int xp) {
    this->xp = xp;
}
void Caracter::setXpReqForNextLevel(int xpReqForNextLevel) {
    this->xpReqForNextLevel = xpReqForNextLevel;
}
void Caracter::up (int xp) {
    this->xp += xp;
    if (this->xp >= xpReqForNextLevel) {
        level++;
        this->xp = this->xp - xpReqForNextLevel;
        xpReqForNextLevel *= 2;
    }
}
int Caracter::getCurrentXp () {
    return xp;
}
int Caracter::getXpReqForNextLevel () {
    return xpReqForNextLevel;
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
    //anims[currentAnimIndex]->setCurrentTile(0);
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
    anims[currentAnimIndex]->updateTransform();
}
Vec2f Caracter::getDir () {
    return dir;
}
void Caracter::setMoving (bool b) {
    this->moving = b;
    if (moving) {
        anims[currentAnimIndex]->play(true);
    } else {
        anims[currentAnimIndex]->stop();
        anims[currentAnimIndex]->setCurrentTile(0);
    }
}
bool Caracter::isMoving () {
    return moving;
}
Vec2f Caracter::getPosition () {
    return Vec2f(anims[currentAnimIndex]->getPosition().x, anims[currentAnimIndex]->getPosition().y);
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
unsigned int Caracter::getCurrentPathIndex() {
    return currentPointIndex;
}
void Caracter::setCurrentPathIndex (unsigned int currentPointIndex) {
    this->currentPointIndex = currentPointIndex;
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
void Caracter::setIsMovingFromKeyboard(bool b) {
    moveFromKeyboard = b;
}
bool Caracter::isMovingFromKeyboard() {
    return moveFromKeyboard;
}
Caracter::~Caracter() {
}



