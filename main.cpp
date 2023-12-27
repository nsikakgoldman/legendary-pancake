#include <gtkmm.h>
#include <X11/Xlib.h>
#include <iostream>
#include <chrono>

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
    void on_start_clicked() {
        if (!isTaskActive) {
            isTaskActive = true;
            startTime = std::chrono::steady_clock::now();
            std::cout << "Task started" << std::endl;
        }
    }

    void on_pause_clicked()
    {
        if (isTaskActive && !isTaskPaused)
        {
            isTaskPaused = true;
            auto endTime = std::chrono::steady_clock::now();
            auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();
            std::cout << "Task paused. Elapsed time: " << elapsedTime << " seconds" << std::endl;
        }
    }

    void on_stop_clicked() {
        if (isTaskActive) {
            isTaskActive = false;
            auto endTime = std::chrono::steady_clock::now();
            auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() / 1000.0; // Convert to seconds
            std::cout << "Task stopped. Elapsed time: " << elapsedTime << " seconds" << std::endl;
        }
    }


public:
    bool on_timeout() {
        // Check for user activity using Xlib
        checkForUserActivity();

        // If there's no activity for a certain period, freeze the time
        auto currentTime = std::chrono::steady_clock::now();
        auto idleTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastActivityTime).count();
        if (idleTime > WAIT_DURATION_SECONDS && isTaskActive) {
            on_pause_clicked();
        }

        return true;
    }

private:
    Gtk::Button startBtn;
    Gtk::Button stopBtn;
    bool isTaskActive = false;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastActivityTime;
    Display* display = nullptr;
    const int IDLE_THRESHOLD_SECONDS = (60 * 10); // Adjust this threshold as needed
    const int WAIT_DURATION_SECONDS = 4 * 60;
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

};

int main(int argc, char** argv) {
    auto app = Gtk::Application::create(argc, argv, "org.gtkmm.examples.base");

    WorkTrackerApp workTrackerApp;

    // Set up a timeout function to check for user inactivity
    Glib::signal_timeout().connect(sigc::mem_fun(workTrackerApp, &WorkTrackerApp::on_timeout), 1000);

    return app->run(workTrackerApp);
}
