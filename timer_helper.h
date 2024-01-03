#ifndef TIMER_HELPER_H
#define TIMER_HELPER_H

#include <iostream>
#include <chrono>

class TimeHelper {
piblic:
    static void printElapsedTime(long seconds);
    static void beep();
};

#endif // TIMER_HELPER_H