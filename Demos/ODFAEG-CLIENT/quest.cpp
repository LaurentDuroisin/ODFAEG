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
}
