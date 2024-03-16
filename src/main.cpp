#include <stdexcept>
#include <iostream>

#include "app.hpp"

int main() {
    VulkanApp app = VulkanApp("Renderer");
    try {
        app.init();
        app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}