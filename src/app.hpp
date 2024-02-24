#pragma once

#include "window.hpp"
#include "platform.hpp"


class VulkanApp {
    public:
        VulkanApp(std::string name);
        void run();
    private:
        void mainLoop();
        /* Responsible for setting up a suitable VkInstance */
        std::string name;

        /* NOTE! Order is important! We rely on destructors for cleanup */

        Platform platform;
        /* Wraps a glfwWindow */
        Window window;
};