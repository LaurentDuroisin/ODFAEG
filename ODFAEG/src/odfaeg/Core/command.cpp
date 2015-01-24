#include "../../../include/odfaeg/Core/command.h"
namespace odfaeg {
    namespace core {
        std::vector<sf::Event> Command::events = std::vector<sf::Event> ();

        Command::Command (Action action, FastDelegate<void> slot) : slot(slot)
        {
            this->action = std::make_unique<Action>(action);
        }

        Command::Command (Action action, FastDelegate<bool> trigger, FastDelegate<void> slot) : slot(slot) {
            this->action = std::make_unique<Action>(action);
            this->trigger = std::make_unique<FastDelegate<bool>>(trigger);
        }
        Command::Command (FastDelegate<bool> trigger, FastDelegate<void> slot) : slot(slot) {
            this->trigger = std::make_unique<FastDelegate<bool>>(trigger);
        }
        Command::Command(const Command& other) : slot(other.slot) {
           if (other.action != nullptr) {
               action = std::make_unique<Action>(*other.action);
           }
           if (other.trigger != nullptr)
               trigger = std::make_unique<FastDelegate<bool>>(*other.trigger);
        }
        bool Command::containsEvent (sf::Event &event)
        {
            std::vector<sf::Event>::iterator it;
            for (it = events.begin(); it != events.end(); it++)
            {
                if (equalEvent(event, *it))
                    return true;
            }
            return false;
        }

        bool Command::isTriggered()
        {
            if (trigger == nullptr && action != nullptr)
            {
                return action->isTriggered();
            }
            if (trigger != nullptr && action != nullptr) {
                return (*trigger)() && action->isTriggered();
            }
            if (trigger != nullptr && action == nullptr) {
                return (*trigger)();
            }
            return false;
        }

        bool Command::containsBufferEvent(sf::Event& event) {
            if (action != nullptr)
                return action->containsEvent(event);
            return false;
        }

        void Command::clearEventsStack ()
        {
            events.clear();
        }

        void Command::pushEvent (sf::Event& event)
        {
            std::vector<sf::Event>::iterator it;
            bool containsEvent = false;
            for (it = events.begin(); it != events.end(); it++)
            {
                if (equalEvent(event, *it))
                    containsEvent = true;
            }
            if (!containsEvent) {
                events.push_back(event);
            }
        }
        Action* Command::getAction() {
            return action.get();
        }
        std::vector<sf::Event> Command::getEvents()
        {
            return events;
        }


        void Command::operator()()
        {
            slot();
        }

        void Command::removeEvent(sf::Event& event) {
            std::vector<sf::Event>::iterator it;
            for (it = events.begin(); it != events.end();) {
                if (equalEvent(*it, event))
                    it = events.erase(it);
                else
                    it++;
            }
        }
        bool Command::equalEvent (sf::Event& event, sf::Event& other) {
            if (event.type != other.type)
                return false;
            if (event.type == sf::Event::Resized) {
                return event.size.width == other.size.width && event.size.height == other.size.height;
            }
            if (event.type == sf::Event::TextEntered) {
                return event.text.unicode == other.text.unicode;
            }
            if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) {
                return event.key.code == other.key.code;
            }
            if (event.type == sf::Event::MouseWheelMoved) {
                return event.mouseWheel.delta == other.mouseWheel.delta;
            }
            if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased) {
                return event.mouseButton.button == other.mouseButton.button;
            }
            if (event.type == sf::Event::MouseMoved) {
                return event.mouseMove.x == other.mouseMove.x && event.mouseMove.y == other.mouseMove.y;
            }
            if (event.type == sf::Event::JoystickButtonPressed || event.type == sf::Event::JoystickButtonReleased) {
                return event.joystickButton.joystickId == other.joystickButton.joystickId
                    && event.joystickButton.button == other.joystickButton.button;
            }
            if (event.type == sf::Event::JoystickMoved) {
                return event.joystickMove.joystickId == other.joystickMove.joystickId
                    && event.joystickMove.position == other.joystickMove.position;
            }
            return false;
        }
        Command& Command::operator=(const Command& other) {
            if (other.action != nullptr)
                action = std::make_unique<Action>(*other.action);
            if (other.trigger != nullptr)
                trigger = std::make_unique<FastDelegate<bool>>(*other.trigger);
            slot = FastDelegate<void>(other.slot);
            return *this;
        }
    }
}

