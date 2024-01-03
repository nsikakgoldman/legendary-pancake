#include "timer_helper.h"
#include <thread>

void TimeHelper::printElapsedTime(long seconds) {
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;

    std::cout << "Task Elapsed time: "<< hours << " hours, " << minutes << " minutes" << std::endl;
}

void TimeHelper::beep(){
    std::cout << '\a' << std::flush; // Print ASCII 7 to produce a beep sound
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}