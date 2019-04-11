namespace odfaeg {
    template <typename... A>
    void Command::setSigParams (A&&... args) {
        if (trigger != nullptr)
            trigger->setParams<A...>(std::forward<A>(args)...);
    }
    template <typename... A>
    void Command::bindSigParams(A&&... args) {
        if (trigger != nullptr)
            trigger->bind(std::forward<A>(args)...);
    }
    template <typename... A>
    void Command::setSlotParams (A&&... args) {
        slot.setParams(std::forward<A>(args)...);
    }
    template <typename... A>
    void Command::bindSlotParams(A&&... args) {
        slot.setParams(std::forward<A>(args)...);
    }
}

