#include <iostream>

#include "app.hpp"
#include "initializers.hpp"

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
    auto commandPoolInfo = vkinit::commandPoolCreateInfo(platform.getQueueFamilyIndex(QueueFamily::GRAPHICS));
    for (int i = 0; i < FRAME_OVERLAP; i++) {
        vkc(vkCreateCommandPool(platform.getDevice(),  &commandPoolInfo, nullptr, &frames[i].commandPool));
        auto cmdAllocInfo = vkinit::commandBufferAllocInfo();
        vkc(vkAllocateCommandBuffers(platform.getDevice(), &cmdAllocInfo, &frames[i].mainCommandBuffer));
    }
};

void VulkanApp::initSyncStructures() {
    auto fenceInfo = vkinit::fenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
    auto semaphoreInfo = vkinit::semaphoreCreateInfo(0);
    for (int i = 0; i < FRAME_OVERLAP; i++) {
        vkc(vkCreateFence(platform.getDevice(), &fenceInfo, nullptr, &frames[i].renderFence));
        vkc(vkCreateSemaphore(platform.getDevice(), &semaphoreInfo, nullptr, &frames[i].renderSemaphore));
        vkc(vkCreateSemaphore(platform.getDevice(), &semaphoreInfo, nullptr, &frames[i].swapchainSemaphore));
    }
}

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