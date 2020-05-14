#include "item.hpp"
using namespace odfaeg::graphic::gui;
namespace sorrok {
    Item::Item(std::string name,Type type) : name(name), type(type) {

    }
    Item::Type Item::getType() {
        return type;
    }
    std::string Item::getName() {
        return name;
    }
    float Item::getAttributeVal(AttribType attribType) {
        std::map<int, float>::iterator it = attributes.find(attribType);
        return (it != attributes.end()) ? it->second : 0;
    }
    void Item::setIcon(Icon* icon) {
        this->icon = icon;
    }
    Icon* Item::getIcon() {
        return icon;
    }
}
