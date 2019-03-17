#ifndef GAME_ACTION_HPP
#define GAME_ACTION_HPP
#include "hero.h"
namespace sorrok {
    class GameAction : public odfaeg::core::Visitor<> {
        public :
        void operator()(const Hero::Novice&, Item& item, odfaeg::core::FastDelegate<void>& action);
        void operator()(const Hero::Warrior&, Item& item, odfaeg::core::FastDelegate<void>& actio);
        void operator()(const Hero::Magician&, Item& item, odfaeg::core::FastDelegate<void>& actio);
        void operator()(const Hero::Thief&, Item& item, odfaeg::core::FastDelegate<void>& actio);
    };
}
#endif
