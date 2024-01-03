#include "screenshot_helper.h"

void ScreenshotHelper::takeScreenshot(Display* display){
    std::ostringstream filename;
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    filename << "screenshot_" << std::put_time(std::localtime(&now), "%Y%m%d_%H%M%S") << ".png";

    xcb_connection_t* connection = xcb_connect(nullptr, nullptr);

    if (connection) {
        xcb_screen_t* screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
        xcb_pixmap_t pixmap = xcb_generate_id(connection);
        xcb_gcontext_t gc = xcb_generate_id(connection);

        xcb_create_pixmap(connection, screen->root_depth, pixmap, screen->root, screen->width_in_pixels,
                            screen->height_in_pixels);

        xcb_create_gc(connection, gc, pixmap, 0, nullptr);

        xcb_copy_area(connection, screen->root, pixmap, gc, 0, 0, 0, 0, screen->width_in_pixels,
                        screen->height_in_pixels);

        xcb_flush(connection);

        std::cout << "Screenshot saved: " << filename.str() << std::endl;
    } else {
        std::cerr << "Failed to connect to X server for taking a screenshot." << std::endl;
    }
}