#ifndef QUEST_HPP
#define QUEST_HPP
class Quest {
    public :
        Quest (std::string name, std::string task);
        std::string getName();
        std::string getTask();
        void addItemToCollect(std::string name, unsigned int nb);
        void addMonsterToKill(std::string name, unsigned int nb);
        bool isMonsterToKillInList(std::string name);
        bool isItemToCollectInList(std::string name);
        void addItemCollectedProgress();
        void addMonsterToKillProgress();
    private :
        std::string name;
        std::string task;
        std::map<std::string, std::pair<unsigned int, unsigned int>> itemsToCollect;
        std::map<std::string, std::pair<unsigned int, unsigned int>> monstersToKill;
};
#endif
