#pragma once
/* TODO: this should not have a glfw dependency */
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace vkinit {
    /* TODO: move more stuff here */
    VkCommandBufferAllocateInfo commandBufferAllocInfo();
    VkCommandPoolCreateInfo commandPoolCreateInfo(VkCommandPoolCreateFlags flags);
    VkFenceCreateInfo fenceCreateInfo(VkFenceCreateFlags flags);
    VkSemaphoreCreateInfo semaphoreCreateInfo(VkSemaphoreCreateFlags flags);
}