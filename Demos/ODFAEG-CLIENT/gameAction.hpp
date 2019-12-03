#ifndef GAME_ACTION_HPP
#define GAME_ACTION_HPP
#include "hero.h"
namespace sorrok {
    class GameAction : public odfaeg::core::Visitor<> {
        public :
        void operator()(const Hero::Novice&, Item& item, Hero* hero);
        void operator()(const Hero::Warrior&, Item& item, Hero* hero);
        void operator()(const Hero::Magician&, Item& item, Hero* hero);
        void operator()(const Hero::Thief&, Item& item, Hero* hero);
        void operator()(const Hero::Novice&, Skill& skill, Hero* hero);
        void operator()(const Hero::Warrior&, Skill& skill, Hero* hero);
        void operator()(const Hero::Magician&, Skill& skill, Hero* hero);
        void operator()(const Hero::Thief&, Skill& skill, Hero* hero);
    };
}
#endif
