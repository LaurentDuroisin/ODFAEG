#ifndef SORROK_SKILL_HPP
#define SORROK_SKILL_HPP
#include <string>
namespace sorrok {
    class Skill {
        public :
        enum Type {
            LAST_HEAL
        };
        enum Stat {
            NONE, HP
        };
        Skill();
        Skill(std::string name, unsigned int damage, std::string target, unsigned int manaCost);
        std::string getName();
        unsigned int getDamage();
        std::string getTarget();
        void setStat(Stat stat);
        Stat getStat();
        unsigned int getManaCost();
        template <typename Archive>
        void serialize (Archive &ar) {
            ar(name);
            ar(target);
            ar(damage);
            ar(stat);
            ar(manaCost);
        }
        private :
        std::string name, target;
        unsigned int damage, manaCost;
        Stat stat;
    };
}
#endif
