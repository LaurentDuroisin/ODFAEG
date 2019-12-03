#include "skill.hpp"
namespace sorrok {
    Skill::Skill() {
        name = "";
        target = "";
        damage = 0;
        stat = NONE;
    }
    Skill::Skill (std::string name, unsigned int damage, std::string target) : name(name), damage(damage), target(target) {
        stat = NONE;
    }
    std::string Skill::getName() {
        return name;
    }
    unsigned int Skill::getDamage() {
        return damage;
    }
    std::string Skill::getTarget() {
        return target;
    }
    void Skill::setStat(Stat stat) {
        this->stat = stat;
    }
    Skill::Stat Skill::getStat() {
        return stat;
    }
}
