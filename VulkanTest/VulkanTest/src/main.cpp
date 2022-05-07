// Base needed libraries
#include <iostream>
#include <stdexcept>
#include <cstdlib>

// Custom Vulkan Application class
#include"HelloTriangleApplication.h"

int main() {
    HelloTriangleApplication app;

    try {
        app.Run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}