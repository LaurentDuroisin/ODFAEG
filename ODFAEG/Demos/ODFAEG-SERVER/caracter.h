#ifndef CARACTER_HPP
#define CARACTER_HPP
#include <vector>
#include "odfaeg/Math/vec2f.h"
#include "odfaeg/Graphics/anim.h"
#include "odfaeg/Graphics/tile.h"
#include <string>
#include <SFML/Graphics.hpp>
class Caracter : public odfaeg::graphic::AnimatedEntity {
public :
    Caracter() : AnimatedEntity(odfaeg::math::Vec3f(0, 0, 0), odfaeg::math::Vec3f(0, 0, 0), odfaeg::math::Vec3f(0, 0, 0),"E_CARACTER") {}
    Caracter (std::string type, std::string name, std::string currentMapName, std::string classs, int level);
    bool isMovable() const {
        return true;
    }
    bool selectable() const {
        return false;
    }
    bool operator== (Entity& other);
    void addAnimation (odfaeg::graphic::Anim *anim);
    odfaeg::graphic::Tile& getCurrentTile();
    void setMoving(bool b);
    bool isMoving ();
    void setDir(odfaeg::math::Vec2f dir);
    odfaeg::math::Vec2f getDir();
    void setPath(std::vector<odfaeg::math::Vec2f> path);
    std::vector<odfaeg::math::Vec2f> getPath();
    float getSpeed();
    void setSpeed(float speed);
    odfaeg::graphic::Anim* getAnimation(unsigned int index);
    unsigned int getCurrentPathIndex ();
    void setCurrentPathIndex (unsigned int index);
    void setRange(int range);
    int getRange();
    void setLife(int life);
    void setMaxLife(int life);
    int getLife ();
    int getMaxLife();
    int getLevel();
    std::string getClass();
    std::string getName();
    int getAttack();
    void setAttack(int attack);
    void setAttackSpeed(float attackSpeed);
    float getAttackSpeed();
    void setFightingMode(bool b);
    bool isInFightingMode();
    bool isAttacking ();
    void setAlive(bool b);
    bool isAlive();
    void setAttacking(bool b);
    void setLevel (int level);
    sf::Time getTimeOfLastAttack();
    sf::Time getTimeOfLastHpRegen();
    void setFocusedCaracter(Caracter* caracter);
    Caracter* getFocusedCaracter();
    void attackFocusedCaracter();
    float getRegenHpSpeed();
    void setRegenHpSpeed(float regenHpSpeed);
    int getRegenHpAmount();
    void setRegenHpAmount(int regenHpAmount);
    Entity* getCurrentFrame() const;
    void onDraw(odfaeg::graphic::RenderTarget&, odfaeg::graphic::RenderStates);
    void onMove(odfaeg::math::Vec3f& t);
    virtual void setIsMovingFromKeyboard(bool b) {}
    virtual bool isMovingFromKeyboard() = 0;
    template <typename Archive>
    void vtserialize(Archive & ar) {
        Entity::vtserialize(ar);
        ar(name);
        ar(currentMapName);
        ar(level);
        ar(speed);
        ar(alive);
        ar(maxLife);
        ar(regenHpAmount);
        ar(attack);
        ar(range);
    }
    virtual bool isMonster() = 0;
    virtual ~Caracter();
private :
    std::string name, currentMapName, classs;
    int level, attack, range;
    float attackSpeed, regenHpSpeed, speed;
    bool moving, alive, moveFromKeyboard;
    odfaeg::math::Vec2f dir;
    std::vector<odfaeg::math::Vec2f> path;
    std::vector<odfaeg::graphic::Anim*> anims;
    int currentAnimIndex;
    int life, maxLife, regenHpAmount;
    bool attacking, fightingMode;
    sf::Clock clockAtkSpeed, clockRegenHp;
    Caracter* focusedCaracter;
};
#endif

