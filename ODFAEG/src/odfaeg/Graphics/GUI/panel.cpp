#include "../../../../include/odfaeg/Graphics/GUI/panel.hpp"
namespace odfaeg {
    namespace graphic {
        namespace gui {
            Panel::Panel(RenderWindow& window, math::Vec3f position, math::Vec3f size, int priority, LightComponent* parent) :
            LightComponent (window, position, size, size * 0.5f, priority, parent) {
                rect = RectangleShape(size);
                background = sf::Color::Black;
                rect.setFillColor(background);
                scrollX = scrollY = false;
                math::Vec3f mousePos (sf::Mouse::getPosition(getWindow()).x, sf::Mouse::getPosition(getWindow()).y, 0);
                oldMouseX = mousePos.x;
                oldMouseY = mousePos.y;
                core::Action a (core::Action::EVENT_TYPE::MOUSE_BUTTON_HELD_DOWN, sf::Mouse::Right);
                core::Command cmd1(a, core::FastDelegate<bool>(&Panel::isOnXScroll, this),core::FastDelegate<void>(&Panel::moveXItems, this));
                getListener().connect("scrollXMove", cmd1);
                core::Command cmd2(a, core::FastDelegate<bool>(&Panel::isOnYScroll, this),core::FastDelegate<void>(&Panel::moveYItems, this));
                getListener().connect("scrollYMove", cmd2);
            }
            bool Panel::isOnXScroll() {
                math::Vec3f mousePos (sf::Mouse::getPosition(getWindow()).x, sf::Mouse::getPosition(getWindow()).y, 0);
                mouseDeltaX = mousePos.x - oldMouseX;
                oldMouseX = mousePos.x;
                if (scrollX) {
                    physic::BoundingBox bb(vertScrollBar.getPosition().x, vertScrollBar.getPosition().y, 0, vertScrollBar.getSize().x, vertScrollBar.getSize().y, 0);
                    return bb.isPointInside(mousePos);
                }
                return false;
            }
            bool Panel::isOnYScroll() {
                math::Vec3f mousePos (sf::Mouse::getPosition(getWindow()).x, sf::Mouse::getPosition(getWindow()).y, 0);
                mouseDeltaY = mousePos.y - oldMouseY;
                oldMouseY  = mousePos.y;
                if (scrollY) {
                    physic::BoundingBox bb(horScrollBar.getPosition().x, horScrollBar.getPosition().y, 0, horScrollBar.getSize().x, horScrollBar.getSize().y, 0);
                    return bb.isPointInside(mousePos);
                }
                return false;
            }
            void Panel::moveXItems() {
                if (mouseDeltaX < 0 && vertScrollBar.getPosition().x - mouseDeltaX >= getPosition().x) {
                    vertScrollBar.move(math::Vec3f(mouseDeltaX, 0, 0));
                    for (unsigned int i = 0; i < children.size(); i++) {
                        children[i]->move(math::Vec3f(-mouseDeltaX, 0, 0));
                    }
                } else if (mouseDeltaX > 0 && vertScrollBar.getPosition().x + vertScrollBar.getSize().x + mouseDeltaX < getPosition().x + getSize().x) {
                    vertScrollBar.move(math::Vec3f(mouseDeltaX, 0, 0));
                    for (unsigned int i = 0; i < children.size(); i++) {
                        children[i]->move(math::Vec3f(-mouseDeltaX, 0, 0));
                    }
                }
            }
            void Panel::moveYItems() {
                if (mouseDeltaY < 0 && horScrollBar.getPosition().y - mouseDeltaY >= getPosition().y) {
                    horScrollBar.move(math::Vec3f(0, horScrollBar.getSize().y, 0));
                    for (unsigned int i = 0; i < getChildren().size(); i++) {
                        getChildren()[i]->move(math::Vec3f(0, -horScrollBar.getSize().y, 0));
                    }
                } else if (mouseDeltaY > 0 && horScrollBar.getPosition().y + horScrollBar.getSize().y + mouseDeltaY < getPosition().y + getSize().y) {
                    horScrollBar.move(math::Vec3f(0, -horScrollBar.getSize().y, 0));
                    for (unsigned int i = 0; i < getChildren().size(); i++) {
                        getChildren()[i]->move(math::Vec3f(0, horScrollBar.getSize().y, 0));
                    }
                }
            }
            void Panel::updateScrolls() {
                maxSize = math::Vec3f(0, 0, 0);
                std::vector<LightComponent*> children = getChildren();
                for (unsigned int i = 0; i < children.size(); i++) {
                    maxSize.y += children[i]->getSize().y;
                    if (children[i]->getSize().x > maxSize.x)
                        maxSize.x = children[i]->getSize().x;
                }
                if (maxSize.x > getSize().x || maxSize.y > getSize().y) {
                    corner = RectangleShape(math::Vec3f(10, 10, 0));
                    corner.setPosition(math::Vec3f(getPosition().x + getSize().x - 10, getPosition().y + getSize().y - 10, 0));
                }
                if (maxSize.x > getSize().x) {
                    float factor = (getSize().x - 10) / maxSize.x;
                    unsigned int scrollXSize = getSize().x * factor;
                    vertScrollBar = RectangleShape(math::Vec3f(scrollXSize, 10, 0));
                    vertScrollBar.setPosition(math::Vec3f(getPosition().x, getPosition().y + getSize().y - 10, 0));
                    scrollX = true;
                } else {
                    scrollX = false;
                }
                if (maxSize.y > getSize().y) {
                    float factor = (getSize().y - 10) / maxSize.y;
                    std::cout<<getSize().y<<" "<<maxSize.y<<" "<<factor<<std::endl;
                    unsigned int scrollYSize = getSize().y * factor;
                    horScrollBar = RectangleShape(math::Vec3f(10, scrollYSize, 0));
                    horScrollBar.setPosition(math::Vec3f(getPosition().x + getSize().x - 10, getPosition().y, 0));
                    horScrollBar.setFillColor(sf::Color(128, 128, 128));
                    scrollY = true;
                } else {
                    scrollY = false;
                }
            }
            void Panel::addChild(LightComponent* component) {
                LightComponent::addChild(component);
                maxSize.y += component->getSize().y;
                if (component->getSize().x > maxSize.x)
                    maxSize.x = component->getSize().x;
                if (maxSize.x > getSize().x || maxSize.y > getSize().y) {
                    corner = RectangleShape(math::Vec3f(10, 10, 0));
                    corner.setPosition(math::Vec3f(getPosition().x + getSize().x - 10, getPosition().y + getSize().y - 10, 0));
                }
                if (maxSize.x > getSize().x) {
                    float factor = (getSize().x - 10) / maxSize.x;
                    unsigned int scrollXSize = getSize().x * factor;
                    vertScrollBar = RectangleShape(math::Vec3f(scrollXSize, 10, 0));
                    vertScrollBar.setPosition(math::Vec3f(getPosition().x, getPosition().y + getSize().y - 10, 0));
                    scrollX = true;
                } else {
                    scrollX = false;
                }
                if (maxSize.y > getSize().y) {
                    float factor = (getSize().y - 10) / maxSize.y;
                    unsigned int scrollYSize = getSize().y * factor;
                    horScrollBar = RectangleShape(math::Vec3f(10, scrollYSize, 0));
                    horScrollBar.setPosition(math::Vec3f(getPosition().x + getSize().x - 10, getPosition().y, 0));
                    scrollY = true;
                } else {
                    scrollY = false;
                }
            }
            void Panel::removeAll() {
                LightComponent::removeAll();
                maxSize = math::Vec3f(0, 0, 0);
                vertScrollBar.setPosition(math::Vec3f(getPosition().x, getPosition().y + getSize().y - 10, 0));
                horScrollBar.setPosition(math::Vec3f(getPosition().x + getSize().x - 10, getPosition().y, 0));
            }
            void Panel::setBackgroundColor(sf::Color background) {
                this->background = background;
            }
            void Panel::clear() {
                if (background != rect.getFillColor()) {
                    rect.setFillColor(background);
                }
                for (unsigned int i = 0; i < getChildren().size(); i++) {
                    getChildren()[i]->clear();
                }
            }
            void Panel::addDrawable(Drawable* drawable) {
                drawables.push_back(drawable);
            }
            void Panel::onDraw(RenderTarget& target, RenderStates states) {
                rect.setPosition(getPosition());
                rect.setSize(getSize());
                target.draw(rect);
                for (unsigned int i = 0; i < drawables.size(); i++) {
                    target.draw(*drawables[i], states);
                }
                if (scrollX || scrollY) {
                    target.draw(corner);
                }
                if (scrollX) {
                    target.draw(vertScrollBar);
                }
                if (scrollY) {
                    target.draw(horScrollBar);
                }
            }
            void Panel::checkSubWindowEvents() {

            }
            void Panel::setBorderThickness(float thickness) {
                rect.setOutlineThickness(thickness);
            }
            void Panel::setBorderColor(sf::Color color) {
                rect.setOutlineColor(color);
            }
        }
    }
}
