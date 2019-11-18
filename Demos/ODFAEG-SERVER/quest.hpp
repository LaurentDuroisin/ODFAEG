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
            void addItemToCollect(std::string, unsigned int nb);
            void addMonsterToKill(std::string, unsigned int nb);
            bool isMonsterToKillInList(std::string name);
            bool isItemToCollectInList(std::string name);
            void addItemCollectedProgress(std::string id);
            void addMonsterToKillProgress(std::string id);
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
            std::map<std::string, std::pair<unsigned int, unsigned int>> itemsToCollect;
            std::map<std::string, std::pair<unsigned int, unsigned int>> monstersToKill;
    };
}
#endif
