namespace odfaeg {
std::vector<sf::Event> Command::events = std::vector<sf::Event> ();

inline Command::Command (Action action, FastDelegate<void> slot)
{
    this->action = new Action(action);
    this->slot = new FastDelegate<void>(slot);
    trigger = nullptr;

}

inline Command::Command (Action action, FastDelegate<bool> trigger, FastDelegate<void> slot) {
    this->action = new Action(action);
    this->slot = new FastDelegate<void>(slot);
    this->trigger = new FastDelegate<bool>(trigger);
}
inline Command::Command (FastDelegate<bool> trigger, FastDelegate<void> slot) {
    action = nullptr;
    this->slot = new FastDelegate<void>(slot);
    this->trigger = new FastDelegate<bool>(trigger);
}

inline bool Command::containsEvent (sf::Event &event)
{
    std::vector<sf::Event>::iterator it;
    for (it = events.begin(); it != events.end(); it++)
    {
        if (equalEvent(event, *it))
            return true;
    }
    return false;
}

inline bool Command::isTriggered()
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

inline bool Command::containsBufferEvent(sf::Event& event) {
    if (action != nullptr)
        return action->containsEvent(event);
    return false;
}

inline void Command::clearEventsStack ()
{
    events.clear();
}

inline void Command::pushEvent (sf::Event& event)
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
inline Action* Command::getAction() {
    return action;
}
inline std::vector<sf::Event> Command::getEvents()
{
    return events;
}


inline void Command::operator()()
{
    (*slot)();
}

inline void Command::removeEvent(sf::Event& event) {
    std::vector<sf::Event>::iterator it;
    for (it = events.begin(); it != events.end();) {
        if (equalEvent(*it, event))
            it = events.erase(it);
        else
            it++;
    }
}
inline bool Command::equalEvent (sf::Event& event, sf::Event& other) {
    if (event.type != other.type)
        return false;
    if (event.type == sf::Event::Resized)
    {
        return event.size.width == other.size.width && event.size.height == other.size.height;
    }
    if (event.type == sf::Event::TextEntered)
    {
        return event.text.unicode == other.text.unicode;
    }
    if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
    {
        return event.key.code == other.key.code;
    }
    if (event.type == sf::Event::MouseWheelMoved)
    {
        return event.mouseWheel.delta == other.mouseWheel.delta;
    }
    if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased)
    {
        return event.mouseButton.button == other.mouseButton.button;
    }
    if (event.type == sf::Event::MouseMoved)
    {
        return event.mouseMove.x == other.mouseMove.x && event.mouseMove.y == other.mouseMove.y;
    }
    if (event.type == sf::Event::JoystickButtonPressed || event.type == sf::Event::JoystickButtonReleased)
    {
        return event.joystickButton.joystickId == other.joystickButton.joystickId
               && event.joystickButton.button == other.joystickButton.button;
    }
    if (event.type == sf::Event::JoystickMoved)
    {
        return event.joystickMove.joystickId == other.joystickMove.joystickId
               && event.joystickMove.position == other.joystickMove.position;
    }
    return false;
}
namespace odfaeg {
    template<typename O, typename ...A>
    inline void Command::setParams(O* object, A... args) {
        slot->setParams(object, args...);
    }
    template <typename ...A>
    inline void Command::setParams (A... args) {
        slot->setParams(args...);
    }
    template <typename O1, typename O2, typename ...A>
    inline void Command::setParams(O1* o1, O2* o2, A... args) {
        trigger->setParams(o1, args...);
        slot->setParams(o2, args...);
    }
    template <typename F, typename O, typename ...A>
    inline void Command::setFuncParams(O* object, A... args) {
        trigger->setParams<F>(object, args...);
    }
    template <typename F1, typename F2, typename O1, typename O2, typename ...A>
    inline void Command::setFuncParams(O1* o1, O2* o2, A... args) {
        trigger->setParams<F1>(o1, args...);
        slot->setParams<F2>(o2, args...);
    }
}

}

