#ifndef QUEST_HPP
#define QUEST_HPP
#include <string>
#include <map>
namespace sorrok {
    class Quest {
        public :
            Quest();
            Quest (std::string name, std::string task);
            std::string getName();
            std::string getTask();
            void addItemToCollect(unsigned int id, unsigned int nb);
            void addMonsterToKill(unsigned int id, unsigned int nb);
            bool isMonsterToKillInList(std::string name);
            bool isItemToCollectInList(std::string name);
            void addItemCollectedProgress(unsigned int id);
            void addMonsterToKillProgress(unsigned int id);
            template <typename Archive>
            void serialize (Archive &ar) {
                ar(name);
                ar(task);
                ar(itemsToCollect);
                ar(monstersToKill);
            }
        private :
            std::string name;
            std::string task;
            std::map<unsigned int, std::pair<unsigned int, unsigned int>> itemsToCollect;
            std::map<unsigned int, std::pair<unsigned int, unsigned int>> monstersToKill;
    };
}
#endif
