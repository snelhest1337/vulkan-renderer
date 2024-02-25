#pragma once
#include <memory>

#include "window.hpp"
#include "platform.hpp"


class VulkanApp {
    public:
        VulkanApp(std::string name);
        void run();
        void teardown();
    private:
        void mainLoop();
        std::string name;
        std::shared_ptr<Window> window;
        Platform platform;
        // Renderer renderer;
};