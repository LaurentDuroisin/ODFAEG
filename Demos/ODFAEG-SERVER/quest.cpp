#include "quest.hpp"
namespace sorrok {
    Quest::Quest() {
    }
    Quest::Quest(std::string name, std::string task) : name(name), task(task) {
    }
    std::string Quest::getName() {
        return name;
    }
    std::string Quest::getTask() {
        return task;
    }
    void Quest::addItemToCollect(unsigned int id, unsigned int nb) {
        itemsToCollect.insert(std::make_pair(id, std::make_pair(nb, 0)));
    }
    void Quest::addMonsterToKill(unsigned int id, unsigned int nb) {
        monstersToKill.insert(std::make_pair(id, std::make_pair(nb, 0)));
    }
    void Quest::addItemCollectedProgress(unsigned int id) {
        std::map<unsigned int, std::pair<unsigned int, unsigned int>>::iterator it;
        it = itemsToCollect.find(id);
        if (it != itemsToCollect.end() && it->second.second < it->second.first) {
            it->second.second++;
        }
    }
    void Quest::addMonsterToKillProgress(unsigned int id) {
        std::map<unsigned int, std::pair<unsigned int, unsigned int>>::iterator it;
        it = monstersToKill.find(id);
        if (it != monstersToKill.end() && it->second.second < it->second.first) {
            it->second.second++;
        }
    }
}
