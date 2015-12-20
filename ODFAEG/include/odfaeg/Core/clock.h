#ifndef ODFAEG_CLOCK_HPP
#define ODFAEG_CLOCK_HPP
#include <chrono>
#include "time.h"
namespace odfaeg {
    namespace core {
        class Clock {
            public :
                Clock();
                void restart();
                Time getElapsedTime();
            private :
                std::chrono::high_resolution_clock::time_point start;
        };
    }
}
#endif
