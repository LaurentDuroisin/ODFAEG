#include "../../../include/odfaeg/Window/x11Window.hpp"
#include <iostream>
#include <chrono>

namespace {
    // Filter the events received by windows (only allow those matching a specific window)
    Bool checkEvent(::Display*, XEvent* event, XPointer userData)
    {
        // Just check if the event matches the window
        return event->xany.window == reinterpret_cast< ::Window >(userData);
    }
}
namespace odfaeg {
    namespace window {
        using namespace sf;
        Display* X11Window::display = nullptr;
        unsigned int X11Window::nbWindowCreated = 0;
        X11Window::X11Window() {
            opened = false;
            inputContext = nullptr;
            inputMethod = nullptr;
            atomClose = 0;
        }
        X11Window::X11Window(unsigned int width, unsigned int height, std::string title) {
            opened = false;
            inputContext = nullptr;
            inputMethod = nullptr;
            atomClose = 0;
            create(width, height, title);
        }
        void X11Window::create (unsigned int width, unsigned int height, std::string title) {
            if (display == nullptr)
                display = XOpenDisplay(nullptr);
            if (display == nullptr) {
                std::cerr<<"Cannot open display!"<<std::endl;
                exit(1);
            }
            screen = DefaultScreen(display);
            window = XCreateSimpleWindow(display, DefaultRootWindow(display),
                                         (DisplayWidth(display, screen) - width) * 0.5f, (DisplayHeight(display, screen) - height) * 0.5f,
                                          width, height, 1, BlackPixel(display, screen), WhitePixel(display, screen));
            std::basic_string<Uint8> utf8Title;
            Utf32::toUtf8(title.begin(), title.end(), std::back_inserter(utf8Title));

            // Set the _NET_WM_NAME atom, which specifies a UTF-8 encoded window title.
            Atom wmName = XInternAtom(display, "_NET_WM_NAME", False);
            Atom useUtf8 = XInternAtom(display, "UTF8_STRING", False);
            XChangeProperty(display, window, wmName, useUtf8, 8,
                            PropModeReplace, utf8Title.c_str(), utf8Title.size());
            // Set the non-Unicode title as a fallback for window managers who don't support _NET_WM_NAME.
            XStoreName(display, window, title.c_str());
            XSelectInput(display, window, FocusChangeMask | ButtonPressMask | ButtonReleaseMask | ButtonMotionMask |
                                          PointerMotionMask | KeyPressMask | KeyReleaseMask | StructureNotifyMask |
                                          EnterWindowMask | LeaveWindowMask);
            // Get the atom defining the close event
            atomClose = XInternAtom(display, "WM_DELETE_WINDOW", false);
            XSetWMProtocols(display, window, &atomClose, 1);

            // Create the input context
            inputMethod = XOpenIM(display, NULL, NULL, NULL);
            if (inputMethod)
            {
                inputContext = XCreateIC(inputMethod,
                                           XNClientWindow, window,
                                           XNFocusWindow,  window,
                                           XNInputStyle,   XIMPreeditNothing  | XIMStatusNothing,
                                           (void*)NULL);
            }
            else
            {
                inputContext = NULL;
            }
            if (!inputContext)
                std::cerr<<"Failed to create input context for window -- TextEntered event won't be able to return unicode" << std::endl;
            XMapWindow(display, window);
            XFlush(display);
            nbWindowCreated++;
            opened = true;
        }
        void X11Window::setPosition(unsigned int x, unsigned int y) {
            XMoveWindow(display, window, x, y);
            XFlush(display);
        }
        void X11Window::setVisible(bool visible) {
            if (visible) {
                XMapWindow(display, window);
            } else {
                XUnmapWindow(display, window);
            }
            XFlush(display);
        }
        bool X11Window::pollEvent(IEvent& event) {
            XEvent xevent;
            if (opened && XCheckIfEvent(display, &xevent, &checkEvent, reinterpret_cast<XPointer>(window))) {
                XNextEvent(display, &xevent);
                if (xevent.type == MapNotify) {
                    printf("Receive map notification\n");
                }
                if (xevent.type == UnmapNotify) {
                    printf("Receive UnmapNotify notification\n");
                }
                if ((xevent.xclient.format == 32) && (xevent.xclient.data.l[0]) == static_cast<long>(atomClose))
                {
                    event.type = IEvent::EventType::WINDOW_EVENT;
                    std::chrono::time_point<std::chrono::system_clock> time = std::chrono::system_clock::now();
                    event.window.type = IEvent::EventType::WINDOW_EVENT;
                    event.window.timestamp = std::chrono::system_clock::to_time_t(time);
                    event.window.windowID = reinterpret_cast<unsigned long long int>(&window);
                    event.window.event = IEvent::WindowEventID::WINDOW_EVENT_CLOSED;
                }
                return true;
            }
            return false;
        }
        bool X11Window::isOpen() {
            return opened;
        }
        void X11Window::close () {
            if (opened) {
                XDestroyWindow(display, window);
                nbWindowCreated--;
                if (nbWindowCreated == 0) {
                    XCloseDisplay(display);
                    display = nullptr;
                    if (inputMethod)
                            XCloseIM(inputMethod);
                }
                opened = false;
            }
        }
        unsigned long long int X11Window::getID() {
            return reinterpret_cast<unsigned long long int>(&window);
        }
        X11Window::~X11Window() {
            if (opened) {
                XDestroyWindow(display, window);
                if (inputMethod)
                        XCloseIM(inputMethod);
                nbWindowCreated--;
                if (nbWindowCreated == 0) {
                    XCloseDisplay(display);
                    display = nullptr;
                }
                opened = false;
            }
        }
    }
}
