#include "../../../include/odfaeg/Driver/command.hpp"
DriverCommand::DriverCommand() : t(DriverCommand::executeAllCommands) {
    commandExecuted.store(true);
}
void DriverCommand::addCommand(FastDelegate<void> delegate) {
    commands.addCommand(delegate);
    commandExecuted.store(false);
}
bool DriverCommand::isCommandExecuted() {
    return commandExecuted.load();
}
void DriverCommand::executeAllCommands () {
    running.store(true);
    while(running.load()) {
        if (!commandExecuted.load()) {
            for (unsigned int i = 0; i < commands.size(); i++) {
                commands[i]();
            }
            commandExecuted.store(true);
        }
    }
}
DriverCommand::~DriverCommand() {
    running.store(false);
}
