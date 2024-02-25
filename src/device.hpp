#pragma once
/* TODO: should not have glfw-dependency */
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <optional>
#include <cstdint>

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct DeviceArgs {
    VkInstance instance;
    VkSurfaceKHR surface;
    bool useValidationLayers;
};

/*
 * Holds logical device & physical device,
 * and some helpers for getting information about the physicalDevice.
 * Platform should already have initialized an instance, this should
 * wrap up device-specific interactions and contain the logical device reference.
 */
class Device {
    public:
        Device();
        void init(const DeviceArgs &args);
        void destroy();
        VkDevice get();
        /* Will be needed by allocator */
        // uint32_t getMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    private:
        QueueFamilyIndices getQueueFamilyIndices(VkPhysicalDevice physicalDevice);
        void createLogicalDevice();
        void selectPhysicalDevice(VkInstance instance);
        bool isDeviceSuitable(VkPhysicalDevice device);
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
        QueueFamilyIndices qIndices;
        VkPhysicalDevice physicalDevice;
        VkDevice device;
        VkQueue graphicsQueue;
        VkQueue presentQueue;
        bool useValidationLayers;
        VkSurfaceKHR surface;
};