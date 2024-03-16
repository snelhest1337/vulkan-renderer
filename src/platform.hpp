#pragma once
/* TODO: this should not have a glfw dependency */
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <functional>
#include <memory>

#include "common.hpp"
#include "window.hpp"
#include "device.hpp"
#include "swapchain.hpp"

/* TODO: Required extensions in the platform arguments */
struct PlatformArgs {
    std::string name;
    bool useValidationLayers;
    std::shared_ptr<Window> window;
};

/* TODO: smarter queue management */
enum class QueueFamily {
    GRAPHICS,
    PRESENT,
};

class Platform {
    public:
        void init(const PlatformArgs &args);
        void destroy();
        /* Unused? Move to device. */
        uint32_t getQueueFamilyIndex(QueueFamily family);
        std::shared_ptr<Device> device;
        std::shared_ptr<SwapChain> swapChain;
        VkInstance instance;
    private:
        std::vector<const char*> getRequiredExtensions();
        bool checkValidationLayerSupport();
        void createInstance();
        void setupDebugMessenger();

        /* Holds platform arguments */
        bool useValidationLayers;
        std::string name;
        std::shared_ptr<Window> window;

        /* Internal */
        VkDebugUtilsMessengerEXT debugMessenger;
        VkSurfaceKHR surface;
};