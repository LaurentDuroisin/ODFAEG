#include "gameAction.hpp"
using namespace odfaeg::core;
namespace sorrok {
    void GameAction::operator()(const Hero::Novice&, Item& item, FastDelegate<void>& action) {
        std::cout<<"action"<<std::endl;
        if (item.getType() == Item::HP_POTION) {
            std::cout<<"Novice take hp potion"<<std::endl;
            action();
        }
    }
    void GameAction::operator()(const Hero::Warrior&, Item& item, FastDelegate<void>& action) {
    }
    void GameAction::operator()(const Hero::Magician&, Item& item, FastDelegate<void>& action) {
    }
    void GameAction::operator()(const Hero::Thief&, Item& item, FastDelegate<void>& action) {
    }
}
