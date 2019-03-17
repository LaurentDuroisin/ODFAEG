#include "item.hpp"
namespace sorrok {
    Item::Item(std::string name,Type type) : name(name), type(type) {

    }
    Item::Type Item::getType() {
        return type;
    }
    std::string Item::getName() {
        return name;
    }
    void Item::addAttribute(AttribType attribType, float value) {
        attributes.insert(std::make_pair(attribType, value));
    }
}
