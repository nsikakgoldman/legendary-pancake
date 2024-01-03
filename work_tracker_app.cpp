#include "work_tracker_app.h"

WorkTrackerApp::WorkTrackerApp() {
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

WorkTrackerApp::~WorkTrackerApp() {
// Close the X display
    if (display) {
        XCloseDisplay(display);
    }
}

void WorkTrackerApp::checkForUserActivity() {
    XEvent ev;
    while (XPending(display) > 0)
    {
        /* code */
        XNextEvent(display, &ev);
        handleXEvent(ev);
    }
}

void WorkTrackerApp::handleXEvent(const XEvent& ev) {
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

void WorkTrackerApp::detectScreenState() {
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