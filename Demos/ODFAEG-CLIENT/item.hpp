#ifndef ITEM_HPP
#define ITEM_HPP
#include <string>
#include <map>
#include "odfaeg/Graphics/GUI/icon.hpp"
namespace sorrok {
    class Item {
    public :
        enum Type {
            HP_POTION
        };
        enum AttribType {
            POTION_AMOUNT
        };
        Item () {}
        Item (std::string name, Type type);
        Type getType();
        std::string getName();
        float getAttributeVal(AttribType attribType);
        template <typename Archive>
        void serialize(Archive& ar) {
            ar(name);
            ar(type);
            ar(attributes);
        }
        void setIcon(odfaeg::graphic::gui::Icon* icon);
        odfaeg::graphic::gui::Icon* getIcon();
    private :
        std::string name;
        Type type;
        std::map<int, float> attributes;
        odfaeg::graphic::gui::Icon* icon;
    };
}
#endif // ITEM_HPP
