#include "quest.hpp"
namespace sorrok {
    Quest::Quest(std::string name, std::string task) : name(name), task(task) {
    }
    std::string Quest::getName() {
        return name;
    }
    std::string Quest::getTask() {
        return task;
    }
    void Quest::addItemToCollect(std::string name, unsigned int nb) {
        itemsToCollect.insert(std::pair(name, std::pair(nb, 0)));
    }
    void Quest::addMonsterToKill(std::string name, unsigned int nb) {
        monstersToKill.insert(std::pair(name, std::pair(nb, 0));
    }
    void Quest::addItemCollectedProgress(std::string name) {
        std::map<std::string, std::pair<unsigned int, unsigned int>>::iterator it;
        it = itemsToCollect.find(name);
        if (it != itemsToCollect.end() && it->second.second < it->second.first) {
            it->second.second++;
        }
    }
    void Quest::addMonsterToKillProgress(std::string name) {
        std::map<std::string, std::pair<unsigned int, unsigned int>>::iterator it;
        it = monstersToKill.find(name);
        if (it != monstersToKill.end() && it->second.second < it->second.first) {
            it->second.second++;
        }
    }
}
