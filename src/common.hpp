#pragma once
#include <format>
#include <vector>
/* TODO: this should not have a glfw dependency */
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define SECOND 1000000000

/* TODO: this should be passed around in a smarter way */
const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};
const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

/* macro to throw runtime error when not returning VK_SUCCESS */
#define vkc(x) if( x != VK_SUCCESS) do {                                                    \
    VkResult ret = (x);                                                                     \
    if (ret != VK_SUCCESS) {                                                                \
        throw std::runtime_error(std::format("ERROR: {} at {}:{}", #x, __FILE__, __LINE__));\
    }                                                                                       \
} while(0);
