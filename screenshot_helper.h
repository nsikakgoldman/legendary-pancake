#ifndef SCREENSHOT_HELPER_H
#define SCREENSHOT_HELPER_H

#include <X11/Xlib.h>
#include <X11/X.h>
#include <xcb/xcb.h>
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>

class ScreenshotHelper {
public:
    static void takeScreenshot(Display* display);
}

#endif // SCREENSHOT_HELPER_H