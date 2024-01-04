#include <cstdlib>
#include <iostream>

void captureScreenshot(const char *filename) {
    // Use scrot to capture a screenshot
    std::string command = "scrot " + std::string(filename);
    int result = std::system(command.c_str());

    if (result != 0) {
        std::cerr << "Failed to capture screenshot." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Screenshot saved to " << filename << std::endl;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <output_filename>" << std::endl;
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];

    captureScreenshot(filename);

    return EXIT_SUCCESS;
}
