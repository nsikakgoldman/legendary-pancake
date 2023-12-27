#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <iostream>
#include <cstring>

void takeScreenshot(const char* filename) {
    Display* display = XOpenDisplay(nullptr);

    if (!display) {
        std::cerr << "Error: Unable to open display." << std::endl;
        return;
    }

    Window root = DefaultRootWindow(display);

    XWindowAttributes windowAttributes;
    XGetWindowAttributes(display, root, &windowAttributes);

    XImage* img = XGetImage(display, root, 0, 0, windowAttributes.width, windowAttributes.height, AllPlanes, ZPixmap);

    if (!img) {
        std::cerr << "Error: Unable to get image." << std::endl;
        XCloseDisplay(display);
        return;
    }

    FILE* file = fopen(filename, "wb");

    if (!file) {
        std::cerr << "Error: Unable to open file for writing." << std::endl;
        XDestroyImage(img);
        XCloseDisplay(display);
        return;
    }

    fwrite(img->data, 1, img->bytes_per_line * windowAttributes.height, file);

    fclose(file);

    XDestroyImage(img);
    XCloseDisplay(display);

    std::cout << "Screenshot saved as: " << filename << std::endl;
}

int main() {
    const char* filename = "screenshot.xwd"; // You can change the filename and format as needed
    takeScreenshot(filename);

    return 0;
}
