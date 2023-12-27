#include <cstdlib>
#include <iostream>

int main() {
    // Use the gnome-screenshot command to take a screenshot
    // You can customize the command with options as needed
    const char* command = "gnome-screenshot -f screenshot.png";

    // Execute the command
    int result = std::system(command);

    // Check if the command was executed successfully
    if (result == 0) {
        std::cout << "Screenshot taken successfully." << std::endl;
    } else {
        std::cerr << "Failed to take a screenshot." << std::endl;
    }

    return 0;
}
