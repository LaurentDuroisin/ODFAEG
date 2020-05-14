#include "gameAction.hpp"
#include "odfaeg/Network/network.h"
#include "application.h"
using namespace odfaeg::core;
using namespace odfaeg::network;
namespace sorrok {
    void GameAction::operator()(const Hero::Novice&, Item& item, Hero* hero) {
        std::cout<<"action"<<std::endl;
        if (item.getType() == Item::HP_POTION) {
            float potionAmount = item.getAttributeVal(Item::POTION_AMOUNT);
            static_cast<MyAppli*>(MyAppli::app)->retractFromInventory(item);
            /*std::cout<<"hp potion amount : "<<potionAmount<<std::endl;
            if (hero->getLife() + potionAmount > hero->getMaxLife()) {
                hero->setLife(hero->getMaxLife());
            } else {
                hero->setLife(hero->getLife() + potionAmount);
            }*/
            SymEncPacket packet;
            std::string request = "ADDLIFE*"+conversionIntString(hero->getId())+"*"+conversionFloatString(potionAmount);
            packet<<request;
            Network::sendTcpPacket(packet);
        }
    }
    void GameAction::operator()(const Hero::Warrior&, Item& item, Hero* hero) {
    }
    void GameAction::operator()(const Hero::Magician&, Item& item, Hero* hero) {
    }
    void GameAction::operator()(const Hero::Thief&, Item& item, Hero* hero) {
    }
    void GameAction::operator()(const Hero::Novice&, Skill& skill, Hero* hero) {
        if (hero->getMana() >= skill.getManaCost()) {
            //hero->setMana(hero->getMana() - skill.getManaCost());
            if (skill.getTarget() == "SELF") {
                if (skill.getStat() == Skill::HP && hero->isAlive()) {
                    //hero->setLife(hero->getLife() + skill.getDamage());
                    static_cast<MyAppli*>(MyAppli::app)->launchSkillAnim(skill.getName());
                    std::string request = "ADDLIFE*"+conversionIntString(hero->getId())+"*"+conversionFloatString(skill.getDamage());
                    SymEncPacket packet;
                    packet<<request;
                    Network::sendTcpPacket(packet);
                    request = "RETRACTMANA*"+conversionIntString(hero->getId())+"*"+conversionFloatString(skill.getManaCost());
                    packet.clear();
                    packet<<request;
                    Network::sendTcpPacket(packet);
                }
            }
        }
    }
    void GameAction::operator()(const Hero::Warrior&, Skill& skill, Hero* hero) {
    }
    void GameAction::operator()(const Hero::Magician&, Skill& skill, Hero* hero) {
    }
    void GameAction::operator()(const Hero::Thief&, Skill& skill, Hero* hero) {
    }
}
