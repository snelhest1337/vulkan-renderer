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
        bool shouldClose();
        void pollEvents();
        ~Window();
    private:
        unsigned width;
        unsigned height;
        std::string name;
        GLFWwindow *window;
};