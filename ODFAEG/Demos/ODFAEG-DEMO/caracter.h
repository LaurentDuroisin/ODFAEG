// *** ADDED BY HEADER FIXUP ***
#include <vector>
// *** END ***
#ifndef CARACTER
#define CARACTER
#include "odfaeg/Math/vec2f.h"
#include "odfaeg/Graphics/anim.h"
#include "odfaeg/Graphics/tile.h"
#include <string>
#include <SFML/Graphics.hpp>
class Caracter : public odfaeg::graphic::AnimatedEntity {
public :
    Caracter() : AnimatedEntity(odfaeg::math::Vec3f(0, 0, 0), odfaeg::math::Vec3f(0, 0, 0), odfaeg::math::Vec3f(0, 0, 0),"E_CARACTER") {}
    Caracter (std::string factionName, std::string pseudo, std::string sex, std::string currentMapName, std::string hairColor,
    std::string eyesColor, std::string skinColor, std::string faceType, std::string classs, int level);
    bool selectable() const {
        return false;
    }
    odfaeg::math::Vec2f getPosition ();
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
    bool isMonster() {
        return false;
    }
    template <typename Archive>
    void vtserialize(Archive & ar) {
        Entity::vtserialize(ar);
        ar(factionName);
        ar(pseudo);
        ar(sex);
        ar(currentMapName);
        ar(hairColor);
        ar(eyesColor);
        ar(skinColor);
        ar(faceType);
        ar(classs);
        ar(level);
        ar(speed);
    }
    void setRange(int range);
    int getRange();
    void setLife(int life);
    void setMaxLife(int life);
    int getLife ();
    int getMaxLife();
    int getLevel();
    std::string getClass();
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
    void setCurrentXp(int xp);
    void setXpReqForNextLevel(int xpReqForNextLevel);
    void setLevel (int level);
    sf::Time getTimeOfLastAttack();
    sf::Time getTimeOfLastHpRegen();
    void attackFocusedMonster();
    void up (int xp);
    int getCurrentXp ();
    int getXpReqForNextLevel ();
    float getRegenHpSpeed();
    void setRegenHpSpeed(float regenHpSpeed);
    int getRegenHpAmount();
    void setRegenHpAmount(int regenHpAmount);
    Entity* getCurrentEntity() const;
    void onDraw(odfaeg::graphic::RenderTarget&, odfaeg::graphic::RenderStates) const;
    void onMove(odfaeg::math::Vec3f& t);
    void setIsMovingFromKeyboard(bool b);
    bool isMovingFromKeyboard();
    virtual ~Caracter();
    private :
    std::string factionName, pseudo, sex, currentMapName, hairColor, eyesColor, skinColor, faceType, classs;
    int level, currentPointIndex, attack, range;
    float attackSpeed, regenHpSpeed, speed;
    bool moving, alive, moveFromKeyboard;
    odfaeg::math::Vec2f dir;
    std::vector<odfaeg::math::Vec2f> path;
    std::vector<odfaeg::graphic::Anim*> anims;
    int currentAnimIndex;
    int life, maxLife, xp, xpReqForNextLevel, regenHpAmount;
    bool attacking, fightingMode;
    sf::Clock clockAtkSpeed, clockRegenHp;
};
#endif
