#ifndef ITEM_HPP
#define ITEM_HPP
#include <string>
namespace sorrok {
    class Item {
    public :
        enum Type {
            HP_POTION
        };
        Item () {}
        Item (std::string name, Type type);
        Type getType();
        std::string getName();
        template <typename Archive>
        void serialize(Archive& ar) {
            ar(name);
            ar(type);
        }
    private :
        std::string name;
        Type type;
    };
}
#endif // ITEM_HPP
