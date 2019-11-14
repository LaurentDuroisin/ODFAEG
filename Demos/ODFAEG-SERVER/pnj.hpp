#ifndef SORROK_PNJ
#define SORROK_PNJ
#include "caracter.h"
#include <vector>
#include "quest.hpp"
namespace sorrok {
    class Pnj : public Caracter {
        public :
            Pnj();
            void addQuest(Quest quest);
            bool isMovingFromKeyboard();
            bool isMonster();
        private :
            std::vector<Quest> quests;
    };
}
#endif
