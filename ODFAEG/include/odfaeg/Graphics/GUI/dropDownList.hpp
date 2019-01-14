#ifndef DROP_DOWN_LIST
#define DROP_DOWN_LIST
#include "../rectangleShape.h"
#include "../convexShape.h"
#include "../renderWindow.h"
#include "../lightComponent.h"
#include "../../Math/vec4.h"
#include "../text.h"
#include "label.hpp"
#include "../../../../include/odfaeg/Physics/boundingPolyhedron.h"
namespace odfaeg {
    namespace graphic {
        namespace gui {
            class DropDownList : public LightComponent {
                public :
                    DropDownList(RenderWindow& window, math::Vec3f position, math::Vec3f size, const Font* font, std::string text, unsigned int charSize);
                    void clear();
                    void addItem(std::string t, unsigned int charSize);
                    void onDraw (RenderTarget& target, RenderStates states = RenderStates::Default);
                    void onUpdate(RenderWindow* window, sf::Event& event);
                    bool isMouseOnTriangle();
                    void onTriangleClicked();
                    void onItemSelected(Label* label);
                    void onEventPushed(sf::Event event, RenderWindow& window);
                    void recomputePos();
                    bool isDroppedDown();
                    bool isValueChanged();
                    std::string getSelectedItem();
                    ~DropDownList();
                private :
                    math::Vec3f selectedItemPos;
                    bool dropDown;
                    const Font* font;
                    bool valueChanged;
                    unsigned int nbItems;
                    math::Vec3f mousePos;
                    RectangleShape rect;
                    ConvexShape shape;
                    physic::BoundingPolyhedron bp;
                    std::vector<Label*> items;
                    Label* selectedItem;
                    sf::Color background;
            };
        }
    }
}
#endif
