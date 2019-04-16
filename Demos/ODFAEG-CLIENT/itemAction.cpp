#include "itemAction.hpp"
namespace sorrok {
    using namespace odfaeg::network;
    using namespace odfaeg::core;
    void ItemAction::useHpPotion(Hero* hero, Item hpPotion) {
        std::cout<<"use hp potion action"<<std::endl;
        float potionAmount = hpPotion.getAttributeVal(Item::POTION_AMOUNT);
        std::cout<<"hp potion amount : "<<potionAmount<<std::endl;
        if (hero->getLife() + potionAmount > hero->getMaxLife()) {
            hero->setLife(hero->getMaxLife());
        } else {
            hero->setLife(hero->getLife() + potionAmount);
        }
        SymEncPacket packet;
        std::string request = "ADDLIFE*"+conversionIntString(hero->getId())+"*"+conversionFloatString(potionAmount);
        packet<<request;
        Network::sendTcpPacket(packet);
    }
}
