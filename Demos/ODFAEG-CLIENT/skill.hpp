#ifndef SORROK_SKILL_HPP
#define SORROK_SKILL_HPP
#include <string>
namespace sorrok {
    class Skill {
        public :
        enum Stat {
            NONE, HP
        };
        Skill();
        Skill(std::string name, unsigned int damage, std::string target);
        std::string getName();
        unsigned int getDamage();
        std::string getTarget();
        void setStat(Stat stat);
        Stat getStat();
        private :
        std::string name, target;
        unsigned int damage;
        Stat stat;
    };
}
#endif
