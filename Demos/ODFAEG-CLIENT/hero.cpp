#include "hero.h"
namespace sorrok {
    using namespace odfaeg::core;
    using namespace odfaeg::graphic::gui;
    Hero::Hero(std::string factionName, std::string name, std::string sex, std::string currentMapName, std::string hairColor,
        std::string eyesColor, std::string skinColor, std::string faceType, std::string classs, int level)
        : Caracter("E_HERO", name, currentMapName, classs, level) {
        this->factionName = factionName;
        this->sex = sex;
        this->hairColor = hairColor;
        this->eyesColor = eyesColor;
        this->skinColor = skinColor;
        this->faceType = faceType;
        xp = 0;
        xpReqForNextLevel = 1500;
        moveFromKeyboard = false;
        job = Hero::Job::Novice;
    }
    void Hero::addItem(Item item) {
        std::map<Item::Type, std::vector<Item>>::iterator it;
        it = inventory.find(item.getType());
        if (it != inventory.end()) {
            it->second.push_back(item);
        } else {
            std::vector<Item> items;
            items.push_back(item);
            inventory.insert(std::make_pair(item.getType(), items));
        }
    }
    bool Hero::isMovingFromKeyboard() {
        return moveFromKeyboard;
    }
    void Hero::setCurrentXp(int xp) {
        this->xp = xp;
    }
    void Hero::setXpReqForNextLevel(int xpReqForNextLevel) {
        this->xpReqForNextLevel = xpReqForNextLevel;
    }
    void Hero::up (int xp) {
        this->xp += xp;
        if (this->xp >= xpReqForNextLevel) {
            setLevel(getLevel() + 1);
            this->xp = this->xp - xpReqForNextLevel;
            xpReqForNextLevel *= 1.1f;
        }
    }
    int Hero::getCurrentXp () {
        return xp;
    }
    int Hero::getXpReqForNextLevel () {
        return xpReqForNextLevel;
    }
    void Hero::setIsMovingFromKeyboard(bool b) {
        moveFromKeyboard = b;
    }
    std::map<Item::Type, std::vector<Item>>& Hero::getInventory() {
        return inventory;
    }
    Variant<Hero::Novice, Hero::Warrior, Hero::Magician, Hero::Thief> Hero::getJobVariant() {
        switch(job) {
            case Hero::Job::Novice : return Variant<Hero::Novice, Hero::Warrior, Hero::Magician, Hero::Thief>(Hero::Novice());
            case Hero::Job::Warrior : return Variant<Hero::Novice, Hero::Warrior, Hero::Magician, Hero::Thief>(Hero::Warrior());
            case Hero::Job::Magician : return Variant<Hero::Novice, Hero::Warrior, Hero::Magician, Hero::Thief>(Hero::Magician());
            case Hero::Job::Thief : return Variant<Hero::Novice, Hero::Warrior, Hero::Magician, Hero::Thief>(Hero::Thief());
        }
    }
    Hero::Job Hero::getJobType() {
        std::cout<<"job : "<<(int) job<<std::endl;
        return job;
    }
    Hero::~Hero() {
    }
}
