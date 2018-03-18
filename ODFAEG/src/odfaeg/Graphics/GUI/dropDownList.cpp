#include "../../../../include/odfaeg/Graphics/GUI/dropDownList.hpp"
namespace odfaeg {
    namespace graphic {
        namespace gui {
            DropDownList::DropDownList(RenderWindow& window, math::Vec3f position, math::Vec3f size, const Font* font, std::string t) :
            LightComponent (window, position, size, size * 0.5f, -1),
            font(font) {
                background = sf::Color::Black;
                if (t != "") {
                    Label* label = new Label(window, position, math::Vec3f(size.x - 50, size.y, 0), font, t);
                    items.push_back(label);
                    nbItems = 1;
                    selectedItem = items[0];
                    core::Action a(core::Action::EVENT_TYPE::MOUSE_BUTTON_HELD_DOWN, sf::Mouse::Left);
                    core::Command cmd (a, core::FastDelegate<bool>(&Label::isMouseInside, items[0]), core::FastDelegate<void>(&DropDownList::onItemSelected, this, items[0]));
                    getListener().connect(items[0]->getText(), cmd);
                    selectedItemPos = items[0]->getPosition();
                } else {
                    nbItems = 0;
                    selectedItem = nullptr;
                }
                rect = RectangleShape(math::Vec3f(size.x-50, size.y, 0));
                shape = ConvexShape(3);
                shape.setPoint(0, sf::Vector3f(size.x - 25, 50, 0));
                shape.setPoint(1, sf::Vector3f(size.x - 50, 0, 0));
                shape.setPoint(2, sf::Vector3f(size.x, 0, 0));
                bp = physic::BoundingPolyhedron(math::Vec3f(position.x + size.x - 25, position.y + 50, 0), math::Vec3f(position.x + size.x - 50, position.y, 0), math::Vec3f(position.x + size.x, position.y, 0),true);
                core::Action a(core::Action::EVENT_TYPE::MOUSE_BUTTON_HELD_DOWN, sf::Mouse::Left);
                core::Command cmd(a, core::FastDelegate<bool> (&DropDownList::isMouseOnTriangle, this), core::FastDelegate<void>(&DropDownList::onTriangleClicked, this));
                getListener().connect("ITEMSELECTED"+t, cmd);
                dropDown = false;
            }
            std::string DropDownList::getSelectedItem() {
                return (selectedItem != nullptr) ? selectedItem->getText() : "";
            }
            void DropDownList::onItemSelected(Label* label) {
                selectedItem = label;
                selectedItemPos = label->getPosition();
                selectedItem->setPosition(getPosition());
                dropDown = false;
            }
            bool DropDownList::isMouseOnTriangle() {
                return bp.isPointInside(mousePos);
            }
            void DropDownList::onTriangleClicked() {
                selectedItem->setPosition(selectedItemPos);
                dropDown = true;
            }
            void DropDownList::clear() {
                if (background != rect.getFillColor())
                    rect.setFillColor(background);
            }
            void DropDownList::addItem(std::string t) {
                Label* label = new Label (getWindow(), getPosition(), math::Vec3f(getSize().x - 50, getSize().y, 0), font, t);
                label->setPosition(math::Vec3f(getPosition().x, getPosition().y + getSize().y * nbItems, 0));
                items.push_back(label);
                nbItems++;
                selectedItem = items[0];
                core::Action a(core::Action::EVENT_TYPE::MOUSE_BUTTON_HELD_DOWN, sf::Mouse::Left);
                core::Command cmd (a, core::FastDelegate<bool>(&Label::isMouseInside, items.back()), core::FastDelegate<void>(&DropDownList::onItemSelected, this, items.back()));
                getListener().connect(items.back()->getText(), cmd);
            }
            void DropDownList::onDraw(RenderTarget& target, RenderStates states) {
                states.transform = getTransform();
                target.draw(rect, states);
                if (!dropDown && selectedItem != nullptr) {
                    target.draw(*selectedItem, states);
                } else {
                    for (unsigned int i = 0; i < items.size(); i++) {
                        target.draw(*items[i], states);
                    }
                }
                target.draw(shape, states);

            }
            void DropDownList::onEventPushed(sf::Event event, RenderWindow& window) {
                if(&window == &getWindow())
                    getListener().pushEvent(event);
            }
            void DropDownList::checkSubWindowEvents () {
            }
            void DropDownList::onUpdate(RenderWindow* window, sf::Event& event) {
                if (&getWindow() == window
                    && event.type == sf::Event::MouseButtonPressed
                    && event.mouseButton.button == sf::Mouse::Left) {
                        mousePos = math::Vec3f(event.mouseButton.x, event.mouseButton.y, 0);
                }
                for (unsigned int i = 0; i < items.size(); i++) {
                    items[i]->onUpdate(window, event);
                }
            }
            DropDownList::~DropDownList() {
                for (unsigned int i = 0; i < items.size(); i++) {
                    delete items[i];
                }
            }
        }
    }
}
