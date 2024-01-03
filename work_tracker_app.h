#ifndef WORK_TRACKER_APP_H
#define WORK_TRACKER_APP_H


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

class WorkTrackerApp : public Gtk::window {
public:
    WorkTrackerApp();
    virtual ~WorkTrackerApp();

protected:
    std::chrono::steady_clock::time_point lastScreenshotTime;
    void on_start_clicked();
    void on_pause_clicked();
    void on_stop_clicked();
    void on_resume_clicked();
    bool on_timeout();
    // ... Existing member functions ...

private:
    // ... Existing private members ...
    Gtk::Button startBtn;
    Gtk::Button stopBtn;
    bool isTaskActive = false;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastActivityTime;
    Display* display = nullptr;

    // New function to handle Xlib events
    void checkForUserActivity();
    void handleXEvent(const XEvent& ev);
    void detectScreenState();
};

#endif // WORK_TRACKER_APP_H