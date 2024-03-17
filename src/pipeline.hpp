#pragma once
/* TODO: this should not have a glfw dependency */
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace vkutil {
    bool loadShaderModule(const char *filePath, VkDevice device, VkShaderModule *module);
}