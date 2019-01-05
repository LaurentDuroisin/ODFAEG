#include "caracter.h"
#include <iostream>
#include "application.h"
using namespace std;
using namespace odfaeg::core;
using namespace odfaeg::graphic;
using namespace odfaeg::physic;
using namespace odfaeg::math;
namespace sorrok {
    Caracter::Caracter (std::string type, std::string name, string currentMapName, string classs, int level) : AnimatedEntity (Vec3f(-25, -50, 0), Vec3f (50, 100, 0), Vec3f(25, 50, 0), type) {
        currentAnimIndex = 0;
        this->name = name;
        this->currentMapName = currentMapName;
        this->classs = classs;
        this->level = level;
        speed = 0.0001f;
        moving = false;
        dir = Vec2f(0, 1);
        this->life = 100;
        this->maxLife = 100;
        range = 100;
        attackSpeed = 1.f;
        attackMin = 5;
        attackMax = 10;
        fightingMode = attacking;
        alive = true;
        regenHpSpeed = 1.f;
        regenHpAmountMin = 1;
        regenHpAmountMax = 2;
        focusedCaracter = nullptr;
        baseAnimIndex = WALKING;
        timeBefLastRespawn = sf::seconds(10.f);
        sf::Int64 i = 0;
        addAttribute("isAlive", i);
        addAttribute("isMoving",i);
        addAttribute("isInFightingMode", i);
        addAttribute("isAttacking", i);
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
    int Caracter::getRegenHpAmountMin() {
        return regenHpAmountMin;
    }
    void Caracter::setRegenHpAmountMin(int regenHpAmount) {
        this->regenHpAmountMin = regenHpAmount;
    }
    int Caracter::getRegenHpAmountMax() {
        return regenHpAmountMax;
    }
    void Caracter::setRegenHpAmountMax(int regenHpAmount) {
        this->regenHpAmountMax = regenHpAmount;
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
        if (attacking != b) {
            this->attacking = b;
            if (attacking) {
                changeAttribute("isAttacking", Application::app->getClock("TimeClock").getElapsedTime().asMicroseconds());
                anims[baseAnimIndex + currentAnimIndex]->stop();
                anims[baseAnimIndex + currentAnimIndex]->setCurrentFrame(0);
                baseAnimIndex = ATTACKING;
                World::update();
            } else {
                changeAttribute("isAttacking", Application::app->getClock("TimeClock").getElapsedTime().asMicroseconds());
                anims[baseAnimIndex + currentAnimIndex]->stop();
                anims[baseAnimIndex + currentAnimIndex]->setCurrentFrame(0);
                damages.clear();
            }
        }
    }
    void Caracter::setAlive(bool b) {
        if (alive == true && b == false) {
            if(getType() == "E_HERO") {
                std::cout<<"hero death"<<std::endl;
            }
            changeAttribute("isAlive",Application::app->getClock("TimeClock").getElapsedTime().asMicroseconds());
            anims[baseAnimIndex + currentAnimIndex]->stop();
            anims[baseAnimIndex + currentAnimIndex]->setCurrentFrame(0);
            baseAnimIndex = TIPPING_OVER;
            anims[baseAnimIndex + currentAnimIndex]->play(false);
            restartRespawn();
            World::update();
        } else if (alive == false && b == true) {
            if(getType() == "E_HERO") {
                std::cout<<"hero alive"<<std::endl;
            }
            changeAttribute("isAlive",Application::app->getClock("TimeClock").getElapsedTime().asMicroseconds());
            damages.clear();
            baseAnimIndex = WALKING;
            setLife(getMaxLife());
            World::update();
        }
        alive = b;
    }
    bool Caracter::isAlive () {
        return alive;
    }
    bool Caracter::isAttacking() {
        return attacking;
    }
    void Caracter::setFightingMode(bool b) {
        if (fightingMode = true && b == false || fightingMode == false && b == true) {
            changeAttribute("isInFightingMode",Application::app->getClock("TimeClock").getElapsedTime().asMicroseconds());
        }
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
    int Caracter::getAttackMin() {
        return attackMin;
    }
    int Caracter::getAttackMax() {
        return attackMax;
    }
    sf::Time Caracter::getTimeOfLastAttack() {
        return clockAtkSpeed.getElapsedTime();
    }
    void Caracter::setDir (Vec2f dir) {
        anims[baseAnimIndex + currentAnimIndex]->stop();
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
        this->dir = dir;
        if (moving)
            anims[baseAnimIndex + currentAnimIndex]->play(true);
        else
            anims[baseAnimIndex + currentAnimIndex]->play(false);
        World::update();
    }
    Vec2f Caracter::getDir () {
        return dir;
    }

    void Caracter::setMoving (bool b) {
        if (moving != b) {
            this->moving = b;
            if (moving) {
                changeAttribute("isMoving",Application::app->getClock("TimeClock").getElapsedTime().asMicroseconds());
                anims[baseAnimIndex + currentAnimIndex]->stop();
                anims[baseAnimIndex + currentAnimIndex]->setCurrentFrame(0);
                baseAnimIndex = WALKING;
                anims[baseAnimIndex + currentAnimIndex]->play(true);
            } else {
                changeAttribute("isMoving",Application::app->getClock("TimeClock").getElapsedTime().asMicroseconds());
                anims[baseAnimIndex + currentAnimIndex]->stop();
                anims[baseAnimIndex + currentAnimIndex]->setCurrentFrame(0);
            }
        }
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
    void Caracter::onDraw(RenderTarget &target, RenderStates states) const {
        target.draw(*getCurrentFrame(), states);
    }
    Entity* Caracter::getCurrentFrame() const {
        return anims[baseAnimIndex + currentAnimIndex]->getCurrentFrame();
    }
    sf::Clock& Caracter::getClkTransfertTime() {
        return clockTransfertTime;
    }
    void Caracter::setFocusedCaracter(Caracter* focusedCaracter) {
        this->focusedCaracter = focusedCaracter;
    }
    Caracter* Caracter::getFocusedCaracter() {
        return focusedCaracter;
    }
    void Caracter::attackFocusedCaracter(int attack, gui::ProgressBar* hpBar, gui::ProgressBar* xpBar) {
        anims[baseAnimIndex + currentAnimIndex]->play(false);
        focusedCaracter->setLife(focusedCaracter->getLife() - attack);
        if (focusedCaracter->getLife() <= 0 && focusedCaracter->isAlive()) {
            focusedCaracter->setLife(0);
        }
        if (focusedCaracter->getType() == "E_HERO")
            hpBar->setValue(focusedCaracter->getLife());
    }
    sf::Time Caracter::getTimeBeforeLastRespawn() {
        return timeBefLastRespawn;
    }
    sf::Time Caracter::getTimeSinceLastRespawn() {
        return clockBeforeLastRespawn.getElapsedTime();
    }
    void Caracter::restartRespawn() {
        clockBeforeLastRespawn.restart();
    }
    void Caracter::setDamages(std::vector<int> damages) {
        this->damages = damages;
    }
    std::vector<int>& Caracter::getDamages() {
        return damages;
    }
    void Caracter::setRegen(std::vector<int> regen) {
        this->regen = regen;
    }
    std::vector<int>& Caracter::getRegen() {
        return regen;
    }
    void Caracter::restartAttackSpeed() {
        clockAtkSpeed.restart();
    }
    void Caracter::restartRegenHP() {
        clockRegenHp.restart();
    }
    Caracter::~Caracter() {
    }
}





