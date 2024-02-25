#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

struct WindowArgs {
    unsigned width;
    unsigned height;
    std::string name;
};

class Window {
    public:
        Window(const WindowArgs &args);
        void init();
        void destroy();
        bool shouldClose();
        void pollEvents();
        void getSize(int &width, int &height);
        VkSurfaceKHR createSurface(VkInstance instance);
    private:
        unsigned width;
        unsigned height;
        std::string name;
        GLFWwindow *window;
        VkSurfaceKHR surface;
};