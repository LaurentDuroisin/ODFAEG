#include "../../../../include/odfaeg/Graphics/GUI/tabPane.hpp"
namespace odfaeg {
    namespace graphic {
        namespace gui {
            TabPane::TabPane(RenderWindow& window, math::Vec3f position, math::Vec3f size) : LightComponent(window,position,size, size * 0.5f) {

            }
            void TabPane::addTab (Panel* panel, std::string tabName, const Font& font) {
                if (getParent() != nullptr) {
                    getParent()->recomputeSize();
                }
                float tabXPos = getPosition().x;
                for (unsigned int i = 0; i < getChildren().size(); i++) {
                    if (dynamic_cast<Label*>(getChildren()[i]) != nullptr) {
                        tabXPos += getChildren()[i]->getSize().x;
                    }
                }
                Label* lTabName = new Label(getWindow(),math::Vec3f(tabXPos,getPosition().y+10,0),math::Vec3f(100,15,0),&font,tabName,15);
                lTabName->setBorderColor(sf::Color::Red);
                lTabName->setBorderThickness(1.f);
                core::Action a (core::Action::MOUSE_BUTTON_PRESSED_ONCE, window::IMouse::Left);
                core::Command cmd (a, core::FastDelegate<bool>(&Label::isMouseInside, lTabName), core::FastDelegate<void>(&TabPane::onTabChanged, this, panel));
                lTabName->getListener().connect("ChangeTab", cmd);
                lTabName->setParent(this);
                panel->setParent(this);
                addChild(lTabName);
                addChild(panel);
                panel->recomputeSize();
                for (unsigned int i = 2; i < getChildren().size(); i++) {
                    if (dynamic_cast<Panel*> (getChildren()[i]) != nullptr) {
                        getChildren()[i]->setVisible(false);
                        getChildren()[i]->setEventContextActivated(false);
                    }
                }
            }
            void TabPane::onTabChanged(Panel* panel) {
                panel->setVisible(true);
                panel->setEventContextActivated(true);
                for (unsigned int i = 0; i < getChildren().size(); i++) {
                    if (dynamic_cast<Panel*> (getChildren()[i]) != nullptr && getChildren()[i] != panel) {
                        getChildren()[i]->setVisible(false);
                        getChildren()[i]->setEventContextActivated(false);
                    }
                }
            }
        }
    }
}
