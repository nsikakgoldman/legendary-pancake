#include <gtkmm.h>
#include <X11/Xlib.h>
#include <X11/X.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <sstream>
#include <iomanip>

class WorkTrackerApp : public Gtk::Window {
public:
    WorkTrackerApp() {
        set_title("Work Tracker");
        set_default_size(300, 200);

        Gtk::Box* vbox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 10));
        startBtn.set_label("Start Task");
        stopBtn.set_label("Stop Task");

        startBtn.signal_clicked().connect(sigc::mem_fun(*this, &WorkTrackerApp::on_start_clicked));
        stopBtn.signal_clicked().connect(sigc::mem_fun(*this, &WorkTrackerApp::on_stop_clicked));

        vbox->pack_start(startBtn, Gtk::PACK_SHRINK);
        vbox->pack_start(stopBtn, Gtk::PACK_SHRINK);

        add(*vbox);

        show_all_children();

        // Initialize Xlib for monitoring user activity
        display = XOpenDisplay(nullptr);
        if (display) {
            // Iterate through all screens and select input on their root windows
            int numScreens = ScreenCount(display);
            for (int i = 0; i < numScreens; ++i) {
                XSelectInput(display, RootWindow(display, i), KeyPressMask | PointerMotionMask);
            }
        }
    }

    virtual ~WorkTrackerApp() {
        // Close the X display
        if (display) {
            XCloseDisplay(display);
        }
    }

protected:
    std::chrono::steady_clock::time_point lastScreenshotTime; // Declare lastScreenshotTime

    void on_start_clicked() {
        if (!isTaskActive) {
            isTaskActive = true;
            startTime = std::chrono::steady_clock::now();
            std::cout << "Task started" << std::endl;
            beep();
        }
    }

    void on_pause_clicked()
    {
        if (isTaskActive && !isTaskPaused)
        {
            isTaskPaused = true;
            auto endTime = std::chrono::steady_clock::now();
            auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();
            printElapsedTime(elapsedTime);
            beep();
        }
    }

    void on_stop_clicked() {
        if (isTaskActive) {
            isTaskActive = false;
            auto endTime = std::chrono::steady_clock::now();
            auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() / 1000.0; // Convert to seconds
            // std::cout << "Task stopped. Elapsed time: " << elapsedTime << " seconds" << std::endl;
            printElapsedTime(elapsedTime);
            beep();
        }
    }

    void on_resume_clicked()
    {
        if (isTaskActive && isTaskPaused)
        {
            isTaskPaused = false;
            lastActivityTime = std::chrono::steady_clock::now();
            std::cout << "Task Resumed"<< std::endl;
            beep();
        }
    }


public:
    bool on_timeout() {
        // Check for user activity using Xlib
        checkForUserActivity();

        // Get the current time
        auto currentTime = std::chrono::steady_clock::now();

        // Calculate idle time
        auto idleTime = std::chrono::duration_cast<std::chrono::minutes>(currentTime - lastActivityTime).count();

        // If there's no activity for a certain period and the task is active
        if (idleTime > WAIT_DURATION_MINUTES) {
            if (isTaskActive && !isTaskPaused) {
                // The user is inactive, so pause the task
                on_pause_clicked();
            }
            // Set isTaskActive to false regardless of whether the task is paused or not
            isTaskActive = false;
        } else {
            // The user is active, so resume the task if it was paused
            if (isTaskActive && isTaskPaused) {
                on_resume_clicked();
            }
        }

        // Take a screenshot every 3 minutes
        if ((std::chrono::steady_clock::now() - lastScreenshotTime) > std::chrono::minutes(3)) {
            takeScreenshot();
            lastScreenshotTime = std::chrono::steady_clock::now();
        }

        // Continue checking for user activity
        return true;  // Change the return type to bool
    }


    void takeScreenshot() {
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

private:
    Gtk::Button startBtn;
    Gtk::Button stopBtn;
    bool isTaskActive = false;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastActivityTime;
    Display* display = nullptr;
    const int IDLE_THRESHOLD_SECONDS = (60 * 10); // Adjust this threshold as needed
    const int WAIT_DURATION_MINUTES = (4 * 60);
    bool isTaskPaused = false;
    
    
    // New function to handle Xlib events
    void checkForUserActivity() {
        XEvent ev;
        while (XPending(display) > 0)
        {
            /* code */
            XNextEvent(display, &ev);
            handleXEvent(ev);
        }
        
    }
    void handleXEvent(const XEvent& ev) {
        switch (ev.type) {
            case KeyPress:
            case KeyRelease:
            case ButtonPress:
            case ButtonRelease:
            case MotionNotify:
                if (isTaskActive || isTaskPaused) {
                    // Only update last activity time when the task is active or paused
                    isTaskActive = true;  // Set isTaskActive to true when user is active
                    isTaskPaused = false; // Reset the paused flag
                    lastActivityTime = std::chrono::steady_clock::now();
                }
                break;
            // Add other event types as needed
        }
    }

    void printElapsedTime(long seconds) {
        int hours = seconds / 3600;
        int minutes = (seconds % 3600) / 60;

        std::cout << "Task Elapsed time: "<< hours << " hours, " << minutes << " minutes" << std::endl; // Wait for a short duration to avoid rapid beeping
    }

    void beep()
    {
        std::cout << '\a' << std::flush; // Print ASCII 7 to produce a beep sound
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    void detectScreenState() {
        // Get the root window attributes to check for screen state
        XWindowAttributes windowAttributes;
        XGetWindowAttributes(display, RootWindow(display, 0), &windowAttributes);

        // Check if the screen is currently in power save mode
        bool screenIsAsleep = (windowAttributes.map_state == IsUnviewable);
        if (screenIsAsleep && isTaskActive && !isTaskPaused)
        {
            // Screen is asleep, pause the task
            on_pause_clicked();   
        } else if (!screenIsAsleep && isTaskActive && isTaskPaused) {
            // Screen is awake, resume the task if it was paused
            on_resume_clicked();
        }
    }

};

int main(int argc, char** argv) {
    auto app = Gtk::Application::create(argc, argv, "org.gtkmm.examples.base");

    WorkTrackerApp workTrackerApp;

    // Set up a timeout function to check for user inactivity
    Glib::signal_timeout().connect(sigc::mem_fun(workTrackerApp, &WorkTrackerApp::on_timeout), 240000);

    

    return app->run(workTrackerApp);
}
