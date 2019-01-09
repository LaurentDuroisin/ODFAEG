#include "../../../include/odfaeg/Core/action.h"
#include "../../../include/odfaeg/Core/command.h"
#include <SFML/Window.hpp>
#include <chrono>
#include <thread>
using namespace std;
namespace odfaeg {
    namespace core {
        Action::Action (EVENT_TYPE type) : type(type) {
            leaf = true;
            pressed = false;
            switch (type) {
                case CLOSED :
                    startEvent.type = sf::Event::Closed;
                    break;
                case RESIZED :
                    startEvent.type = sf::Event::Resized;
                    break;
                case LOST_FOCUS :
                    startEvent.type = sf::Event::LostFocus;
                    break;
                case GAIGNED_FOCUS :
                    startEvent.type = sf::Event::GainedFocus;
                    break;
                case TEXT_ENTERED :
                    startEvent.type = sf::Event::TextEntered;
                    startEvent.text.unicode = 0;
                    break;
                case MOUSE_WHEEL_MOVED :
                    startEvent.type = sf::Event::MouseWheelMoved;
                    break;
                case MOUSE_MOVED :
                    startEvent.type = sf::Event::MouseMoved;
                    break;
                case MOUSE_ENTERED :
                    startEvent.type = sf::Event::MouseEntered;
                    break;
                case MOUSE_LEFT :
                    startEvent.type = sf::Event::MouseEntered;
                    break;
                case JOYSTICK_MOVED :
                    startEvent.type = sf::Event::MouseLeft;
                    break;
                case JOYSTICK_CONNECTED :
                    startEvent.type = sf::Event::JoystickConnected;
                    break;
                case JOYSTICK_DISCONNECTED :
                    startEvent.type = sf::Event::JoystickDisconnected;
                    break;
                default :
                    throw Erreur (1, "Invalid type!", 2);
            }
            is_not = false;
        }
        Action::Action (EVENT_TYPE type, sf::Keyboard::Key key) : type(type) {
            leaf = true;
            pressed = false;
            switch (type) {
            case KEY_PRESSED_ONCE :
                startEvent.type = sf::Event::KeyPressed;
                startEvent.key.code = key;
                break;
            case KEY_HELD_DOWN :
                startEvent.type = sf::Event::KeyPressed;
                startEvent.key.code = key;
                break;
            case KEY_RELEASED :
                startEvent.type = sf::Event::KeyReleased;
                startEvent.key.code = key;
                break;
            default :
                throw Erreur (1, "Invalid type!", 2);
            }
            is_not = false;
        }
        Action::Action (EVENT_TYPE type, sf::Mouse::Button button) : type(type) {
            leaf = true;
            pressed = false;
            switch (type) {
                case MOUSE_BUTTON_PRESSED_ONCE :
                    startEvent.type = sf::Event::MouseButtonPressed;
                    startEvent.mouseButton.button = button;
                    break;
                case MOUSE_BUTTON_HELD_DOWN :
                    startEvent.type = sf::Event::MouseButtonPressed;
                    startEvent.mouseButton.button = button;
                    break;
                case MOUSE_BUTTON_RELEASED :
                    startEvent.type = sf::Event::MouseButtonReleased;
                    startEvent.mouseButton.button = button;
                    break;
                default :
                    throw Erreur (1, "Invalid type!", 2);
            }
            is_not = false;
        }
        Action::Action(const Action& other) {
            leaf = other.leaf;
            pressed = other.pressed;
            startEvent = other.startEvent;
            is_not = other.is_not;
            comparator = other.comparator;
            type = other.type;
            if (!other.leaf) {
                leftChild = std::make_unique<Action>(*other.leftChild);
                rightChild = std::make_unique<Action>(*other.rightChild);
            }
        }
        bool Action::andComparator (Action& a1, Action& a2) {
            return a1.isTriggered() && a2.isTriggered();
        }
        bool Action::orComparator (Action& a1, Action& a2) {
            return a1.isTriggered() || a2.isTriggered();
        }
        bool Action::xorComparator (Action& a1, Action& a2) {
            return a1.isTriggered() | a2.isTriggered();
        }
        void Action::operator! () {
            if (!is_not)
                is_not = true;
            else
                is_not = false;
        }
        Action Action::operator| (Action other) {
            Action result (COMBINED_WITH_XOR, *this, other);
            return result;
        }
        Action Action::operator|| (Action other) {
            Action result (COMBINED_WITH_OR, *this, other);
            return result;

        }
        Action Action::operator&& (Action other) {
            Action result (COMBINED_WITH_AND, *this, other);
            return result;
        }
        bool Action::isTriggered () {

            if (!leaf) {
                return comparator(this, std::ref(*leftChild), std::ref(*rightChild));
            } else {
                /*if (startEvent.type == sf::Event::TextEntered)
                    std::cout<<"text entered : "<<startEvent.text.unicode<<std::endl;*/
                //std::cout<<"triggered"<<std::endl;

                if (type == KEY_HELD_DOWN && !is_not) {
                    return sf::Keyboard::isKeyPressed(startEvent.key.code);
                }
                if (type == KEY_HELD_DOWN && is_not) {
                    return !sf::Keyboard::isKeyPressed(startEvent.key.code);
                }
                if (type == MOUSE_BUTTON_HELD_DOWN && !is_not) {
                    return sf::Mouse::isButtonPressed(startEvent.mouseButton.button);
                }
                if (type == MOUSE_BUTTON_HELD_DOWN && is_not) {
                    return !sf::Mouse::isButtonPressed(startEvent.mouseButton.button);
                }
                /*else if (type == JOYSTICK_BUTTON_HELD_DOWN)
                    return sf::Joystick::isButtonPressed(startEvent.joystickButton.button);*/

                vector<sf::Event> events = Command::getEvents();
                for (unsigned int i = 0; i < events.size(); i++) {
                    /*if (type == KEY_HELD_DOWN || type == MOUSE_BUTTON_HELD_DOWN) {
                        if (!is_not)
                            return Command::equalEvent(events[i], startEvent);
                        else
                            return !Command::equalEvent(events[i], startEvent);
                    } else {*/
                        /*Sometimes the event stored to startEvent is deleted*/
                        if (!is_not && Command::equalEvent(events[i], startEvent) && !pressed) {
                            if (events[i].type == sf::Event::KeyPressed && startEvent.type == sf::Event::KeyPressed
                                || events[i].type == sf::Event::MouseButtonPressed && startEvent.type == sf::Event::MouseButtonPressed)
                                pressed = true;
                            return true;
                        } else if (is_not && !Command::equalEvent(events[i], startEvent) && !pressed) {
                            if (events[i].type == sf::Event::KeyPressed && startEvent.type == sf::Event::KeyPressed
                                || events[i].type == sf::Event::MouseButtonPressed && startEvent.type == sf::Event::MouseButtonPressed)
                                pressed = true;
                            return true;
                        }
                    //}
                }
                return false;

            }
        }
        Action::Action (EVENT_TYPE type, Action leftChild, Action rightChild)  {
            leaf = false;
            this->type = type;
            is_not = false;
            pressed = false;
            if (type == COMBINED_WITH_AND) {
                comparator = &Action::andComparator;
            } else if (type == COMBINED_WITH_OR) {
                comparator = &Action::orComparator;
            } else {
                comparator = &Action::xorComparator;
            }
            this->leftChild = std::make_unique<Action>(leftChild);
            this->rightChild = std::make_unique<Action>(rightChild);


        }

        void Action::setPressed(sf::Event event) {
            if (!leaf) {
                leftChild->setPressed(event);
                rightChild->setPressed(event);
            } else {
                if ((type == KEY_PRESSED_ONCE && event.type == sf::Event::KeyReleased && event.key.code == startEvent.key.code) ||
                    (type == MOUSE_BUTTON_PRESSED_ONCE && event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == startEvent.mouseButton.button)) {
                     pressed = false;
                }
            }
        }
        void Action::getActions(std::vector<Action*>& actions) {
            if (!leaf) {
                leftChild->getActions(actions);
                rightChild->getActions(actions);
            } else {
                actions.push_back(this);
            }
        }
        bool Action::containsEvent(sf::Event& event) {
            if (!leaf) {
                return leftChild->containsEvent(event) || rightChild->containsEvent(event);
            } else {
                return Command::equalEvent(event, startEvent);
            }
        }
        void Action::getEvents(std::vector<sf::Event>& events) {
            if (!leaf) {
                leftChild->getEvents(events);
                rightChild->getEvents(events);
            } else {
                events.push_back(startEvent);
            }
        }
        Action& Action::operator=(const Action& other) {
            leaf = other.leaf;
            pressed = other.pressed;
            startEvent = other.startEvent;
            is_not = other.is_not;
            comparator = other.comparator;
            type = other.type;
            if (!leaf) {
                leftChild = std::make_unique<Action>(*other.leftChild);
                rightChild = std::make_unique<Action>(*other.rightChild);
            }
            return *this;
        }
    }
}

