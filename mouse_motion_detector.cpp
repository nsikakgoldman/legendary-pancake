#include <X11/Xlib.h>
#include <iostream>

int main() {
    Display *display = XOpenDisplay(nullptr);

    if (!display) {
        std::cerr << "Unable to open display." << std::endl;
        return 1;
    }

    Window root = DefaultRootWindow(display);
    XEvent ev;

    XSelectInput(display, root, PointerMotionMask);

    while (true) {
        XNextEvent(display, &ev);
        std::cout<< "hi"<< std::endl;

        if (ev.type == MotionNotify) {
            std::cout << "Mouse motion detected at (x=" << ev.xmotion.x_root
                      << ", y=" << ev.xmotion.y_root << ")" << std::endl;
        }
    }

    XCloseDisplay(display);
    return 0;
}
