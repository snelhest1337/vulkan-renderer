#pragma once
/* TODO: this should not have a glfw dependency */
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace vkutil {
    void transitionImage(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout);
};