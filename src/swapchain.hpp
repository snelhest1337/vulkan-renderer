#pragma once

#include <memory>
#include <vector>
/* TODO: this should not have a glfw dependency */
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "device.hpp"
#include "window.hpp"

struct SwapChainArgs {
    std::shared_ptr<Device> device;
    std::shared_ptr<Window> window;
};

class SwapChain {
    public:
        void init(const SwapChainArgs &args);
        void destroy();
    private:
        void createSwapChain();
        void createImageViews();
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
        std::shared_ptr<Device> device;
        std::shared_ptr<Window> window;
        VkSwapchainKHR swapChain;
        std::vector<VkImage> swapChainImages;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
        /* Should the swapchain contain the image view? Not sure */
        std::vector<VkImageView> swapChainImageViews;
};