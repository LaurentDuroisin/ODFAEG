#include "itemAction.hpp"
namespace sorrok {
    void ItemAction::useHpPotion(Hero* hero, Item hpPotion) {
        std::cout<<"use hp potion action"<<std::endl;
        float potionAmount = hpPotion.getAttributeVal(Item::POTION_AMOUNT);
        std::cout<<"hp potion amount : "<<potionAmount<<std::endl;
        if (hero->getLife() + potionAmount > hero->getMaxLife()) {
            hero->setLife(hero->getMaxLife());
        } else {
            hero->setLife(hero->getLife() + potionAmount);
        }
    }
}
