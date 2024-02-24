#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

#include "common.hpp"

struct PlatformArgs {
    std::string name;
    bool useValidationLayers;

};

class Platform {
    public:
        Platform(const PlatformArgs &args);
        ~Platform();
    private:
        std::vector<const char*> getRequiredExtensions();
        bool checkValidationLayerSupport();
        void createInstance();
        void setupDebugMessenger();

        /* Holds platform arguments */
        bool useValidationLayers;
        std::string name;

        /* Internal */
        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
        const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };
};