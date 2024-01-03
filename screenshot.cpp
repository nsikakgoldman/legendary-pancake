#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/X.h>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <png.h>


void takeScreenshot(const char* filename) {
    Display* display = XOpenDisplay(nullptr);

    if (!display) {
        std::cerr << "Unable to open X display" << std::endl;
        exit(EXIT_FAILURE);
    }

    Window root = DefaultRootWindow(display);

    XWindowAttributes windowAttributes;
    if (!XGetWindowAttributes(display, root, &windowAttributes)) {
        std::cerr << "Unable to get window attributes" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (!windowAttributes.visual) {
        std::cerr << "No visual found for the root window" << std::endl;
        exit(EXIT_FAILURE);
    }

    Visual* visual = DefaultVisual(display, DefaultScreen(display));
    int depth = DefaultDepth(display, DefaultScreen(display));
    XImage* img = XGetImage(display, root, 0, 0, windowAttributes.width, windowAttributes.height, AllPlanes, ZPixmap);
    std::cout << "Visual ID: " << windowAttributes.visual->visualid << std::endl;
    std::cout << "Depth: " << windowAttributes.depth << std::endl;
    
    if (!img) {
        std::cerr << "Unable to capture screen image" << std::endl;
        exit(EXIT_FAILURE);
    }

    FILE* file = fopen(filename, "wb");

    if (!file) {
        std::cerr << "Unable to create file" << std::endl;
        exit(EXIT_FAILURE);
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png) {
        std::cerr << "Unable to create PNG write structure" << std::endl;
        exit(EXIT_FAILURE);
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        std::cerr << "Unable to create PNG info structure" << std::endl;
        exit(EXIT_FAILURE);
    }

    png_init_io(png, file);

    png_set_IHDR(
        png,
        info,
        img->width, img->height,
        8,
        PNG_COLOR_TYPE_RGB,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );

    png_write_info(png, info);

    for (int y = 0; y < img->height; y++) {
        png_write_row(png, reinterpret_cast<png_bytep>(img->data + y * img->bytes_per_line));
    }

    png_write_end(png, nullptr);

    fclose(file);
    XDestroyImage(img);
    XCloseDisplay(display);
}

int main() {
    const char* filename = "screenshot.png";
    takeScreenshot(filename);
    std::cout << "Screenshot saved as " << filename << std::endl;
    return 0;
}
