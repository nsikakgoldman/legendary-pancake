#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>
#include <iostream>

// Callback function for motion events
void handleMotionEvent(XGenericEventCookie* genericEvent) {
    if (genericEvent->type == XI_Motion) {
        std::cout << "Motion event\n";
    }
}

// Callback function for key events
void handleKeyEvent(XGenericEventCookie* genericEvent) {
    if (genericEvent->type == XI_KeyPress) {
        std::cout << "Key event\n";
    }
}

int main() {

    
    Display* display = XOpenDisplay(nullptr);
    if (!display) {
        std::cerr << "Unable to open display.\n";
        return 1;
    }

    Window root = DefaultRootWindow(display);

    // Select motion and key events
    XIEventMask evmask;
    evmask.deviceid = XIAllDevices;

    evmask.mask_len = XIMaskLen(XI_Motion);
    evmask.mask = (unsigned char*)calloc(evmask.mask_len, sizeof(char));
    XISetMask(evmask.mask, XI_Motion);
    XISelectEvents(display, root, &evmask, 1);

    evmask.mask_len = XIMaskLen(XI_KeyPress);
    free(evmask.mask);
    evmask.mask = (unsigned char*)calloc(evmask.mask_len, sizeof(char));
    XISetMask(evmask.mask, XI_KeyPress);
    XISelectEvents(display, root, &evmask, 1);

    XEvent ev;
    XGetEventData(display, &ev.xcookie);

    while (true) {

        switch (ev.xcookie.type) {
            case XI_Motion:
                std::cout << "hi" << std::endl;
                handleMotionEvent(&ev.xcookie);
                break;
            case XI_KeyPress:
                XNextEvent(display, &ev);
                handleKeyEvent(&ev.xcookie);
                break;
            default:
                break;
        }

        XGetEventData(display, &ev.xcookie);
    }

    XCloseDisplay(display);
    return 0;
}
