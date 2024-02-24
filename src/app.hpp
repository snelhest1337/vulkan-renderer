#pragma once

#include "platform.hpp"


class VulkanApp {
    public:
        VulkanApp(std::string name);
        void run();
    private:
        void mainLoop();
        /* Responsible for setting up a suitable VkInstance */
        std::string name;
        Platform platform;

};