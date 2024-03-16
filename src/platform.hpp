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
        uint32_t getQueueFamilyIndex(QueueFamily family);
        VkDevice getDevice();
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
        VkInstance instance;
        std::shared_ptr<Device> device;
        std::shared_ptr<SwapChain> swapChain;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkSurfaceKHR surface;
};