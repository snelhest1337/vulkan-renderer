#include <iostream>

#include "app.hpp"

#define WIDTH 1280
#define HEIGHT 720

VulkanApp::VulkanApp(std::string name):
    name(name),
    window(new Window({.width = WIDTH, .height = HEIGHT, .name = name}))
    /*
     * Not sure about this. Platform hands out unique-ptr to device. Idea is that
     * platform should be responsible for cleaning up the device, and not renderer.
     * Renderer should only have a reference to a device provided and owned by the platform
     */
{
    window->init();
    platform.init({.name = name, .useValidationLayers = true, .window = window});
}

void VulkanApp::initCommands() {
    VkCommandPoolCreateInfo commandPoolInfo = {};
    commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolInfo.pNext = nullptr;
    commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolInfo.queueFamilyIndex = platform.getQueueFamilyIndex(QueueFamily::GRAPHICS);
    for (int i = 0; i < FRAME_OVERLAP; i++) {
        vkc(vkCreateCommandPool(platform.getDevice(),  &commandPoolInfo, nullptr, &frames[i].commandPool));
        VkCommandBufferAllocateInfo cmdAllocInfo = {};
        cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmdAllocInfo.pNext = nullptr;
        cmdAllocInfo.commandBufferCount = 1;
        cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        vkc(vkAllocateCommandBuffers(platform.getDevice(), &cmdAllocInfo, &frames[i].mainCommandBuffer));
    }
};

void VulkanApp::teardown() {
    vkDeviceWaitIdle(platform.getDevice());
    for (int i = 0; i < FRAME_OVERLAP; i++) {
        vkDestroyCommandPool(platform.getDevice(), frames[i].commandPool, nullptr);
    }
    platform.destroy();
    window->destroy();
}

void VulkanApp::run() {
    mainLoop();
}

void VulkanApp::mainLoop() {
    while (!window->shouldClose()) {
        /* Just a wrapper for glfwPollEvents for now */
        window->pollEvents();
    }
    teardown();
}