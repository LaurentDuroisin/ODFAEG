#ifndef ODFAEG_DRIVER_COMMAND_HPP
#define ODFAEG_DRIVER_COMMAND_HPP
#include "../../../include/odfaeg/Core/fastDelegate.h"
#include <thread>
namespace odfaeg {
    namespace driver {
    class DriverCommand {
        public :
            DriverCommand();
            void addCommand (core::FactDelegate<void> fctPtr);
            void executeAllCommands();
            bool isCommandExecuted;
        private :
            std::thread t;
            std::vector<FastDelegate<void>> commands;
            std::atomic<bool> commandExecuted, running;
        }
    };
}
#endif // ODFAEG_DRIVER_COMMAND_HPP
