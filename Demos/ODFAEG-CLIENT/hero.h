#ifndef HERO_HPP
#define HERO_HPP
#include "caracter.h"
#include "item.hpp"
namespace sorrok {
    class Hero : public Caracter {
    public :
        struct Novice {

        };
        struct Warrior {

        };
        struct Magician {

        };
        struct Thief {

        };
        enum class Job {
            Novice, Warrior, Magician, Thief
        };
        Hero() : Caracter("E_HERO", "", "", "", 0), job(Job::Novice) {}
        Hero(std::string factionName, std::string name, std::string sex, std::string currentMapName, std::string hairColor,
        std::string eyesColor, std::string skinColor, std::string faceType, std::string classs, int level);
        void setIsMovingFromKeyboard(bool b);
        bool isMovingFromKeyboard();
        bool isMonster() {
            return false;
        }
        void setCurrentXp(int xp);
        void setXpReqForNextLevel(int xpReqForNextLevel);
        void up (int xp);
        int getCurrentXp ();
        int getXpReqForNextLevel ();
        template <typename Archive>
        void vtserialize(Archive& ar) {
            Caracter::vtserialize(ar);
            ar(factionName);
            ar(sex);
            ar(hairColor);
            ar(eyesColor);
            ar(skinColor);
            ar(faceType);
            ar(xp);
            ar(xpReqForNextLevel);
        }
        void addItem (Item item);
        std::map<Item::Type, std::vector<Item>>& getInventory();
        Job getJobType();
        odfaeg::core::Variant<Hero::Novice, Hero::Warrior, Hero::Magician, Hero::Thief> getJobVariant();
        ~Hero();
    private :
        std::string factionName, sex, hairColor, eyesColor, skinColor, faceType;
        bool moveFromKeyboard;
        int xp, xpReqForNextLevel;
        std::map<Item::Type, std::vector<Item>> inventory;
        Job job;
    };
}
#endif // HERO_HPP
