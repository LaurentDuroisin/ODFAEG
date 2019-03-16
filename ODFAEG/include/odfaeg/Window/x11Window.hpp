#ifndef ODFAEG_X11WINDOW
#define ODFAEG_X11WINDOW
#include <X11/Xlib.h>
#include "iWindow.hpp"
namespace odfaeg {
    namespace window {
        class X11Window : public IWindow {
        public :
            X11Window();
            X11Window(unsigned int width, unsigned int height, std::string title);
            void create (unsigned int width, unsigned int height, std::string title);
            void setPosition(unsigned int x, unsigned int y);
            bool isOpen();
            bool pollEvent (IEvent& event);
            void close();
            void setVisible(bool visible);
            unsigned long long int getID();
            ~X11Window();
        private :
            static Display* display;
            static unsigned int nbWindowCreated;
            Window window;
            int screen;
            bool opened;
            XIM        inputMethod;  ///< Input method linked to the X display
            XIC        inputContext; ///< Input context used to get Unicode input in our window
            Atom       atomClose;    ///< Atom used to identify the close event
        };
    }
}
#endif // ODFAEG_X11WINDOW
