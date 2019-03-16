#ifndef ODFAEG_IWINDOW
#define ODFAEG_IWINDOW
#include <string>
#include "iEvent.hpp"
namespace odfaeg {
    namespace window {
        class IWindow {
        public :
            virtual void create (unsigned int width, unsigned int height, std::string title) = 0;
            virtual void setPosition(unsigned int x, unsigned int y) = 0;
            virtual bool isOpen() = 0;
            virtual bool pollEvent(IEvent& event) = 0;
            virtual void close() = 0;
            virtual void setVisible(bool visible) = 0;
        };
    }
}
#endif // ODFAEG_IWINDOW
