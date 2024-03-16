#include <iostream>
#include <math.h>

#include "app.hpp"
#include "initializers.hpp"
#include "images.hpp"
#include "common.hpp"

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

}

VulkanApp::~VulkanApp() {

}

void VulkanApp::init() {
    frameNumber = 0;
    window->init();
    platform.init({.name = name, .useValidationLayers = true, .window = window});
    initFrameData();
}

void VulkanApp::initFrameData() {
    for (int i = 0; i < FRAME_OVERLAP; i++) {
        FrameData frame;
        frame.init(platform.device);
        frames.push_back(frame);
    }
}

void VulkanApp::teardown() {
    vkDeviceWaitIdle(platform.device->get());
    for (int i = 0; i < FRAME_OVERLAP; i++) {
        frames[i].destroy(platform.device);
    }
    platform.destroy();
    window->destroy();
}

void VulkanApp::draw() {
    auto frame = getCurrentFrame();
    auto swapchain = platform.swapChain->get();
    uint32_t swapchainIndex = 0;

    vkc(vkWaitForFences(platform.device->get(), 1, &frame.renderFence, true, SECOND));
    vkc(vkResetFences(platform.device->get(), 1, &frame.renderFence));
    auto swapchainImage = platform.swapChain->getNextImage(frame.swapchainSemaphore, swapchainIndex);

    auto cmd = frame.mainCommandBuffer;
    vkc(vkResetCommandBuffer(cmd, 0));
    auto cmdBeginInfo = vkinit::commandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    vkc(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

    vkutil::transitionImage(cmd, swapchainImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
    VkClearColorValue clearValue;
    float flash = abs(sin(frameNumber / 120.f));
    clearValue = { {0.0f, 0.0f, flash, 1.0f} };
    auto clearRange = vkinit::imageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

    vkCmdClearColorImage(cmd, swapchainImage, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);

    vkutil::transitionImage(cmd, swapchainImage, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    vkc(vkEndCommandBuffer(cmd));

    auto cmdInfo = vkinit::commandBufferSubmitInfo(cmd);

    auto waitInfo = vkinit::semaphoreSubmitInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, frame.swapchainSemaphore);
    auto signalInfo = vkinit::semaphoreSubmitInfo(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, frame.renderSemaphore);

    auto submit = vkinit::submitInfo(&cmdInfo, &signalInfo, &waitInfo);

    vkc(vkQueueSubmit2(platform.device->getGraphicsQueue(), 1, &submit, frame.renderFence));

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.pSwapchains = &swapchain;
    presentInfo.swapchainCount = 1;

    presentInfo.pWaitSemaphores = &frame.renderSemaphore;
    presentInfo.waitSemaphoreCount = 1;

    presentInfo.pImageIndices = &swapchainIndex;

    vkc(vkQueuePresentKHR(platform.device->getPresentQueue(), &presentInfo));
    frameNumber++;
}

void VulkanApp::run() {
    mainLoop();
}

void VulkanApp::mainLoop() {
    while (!window->shouldClose()) {
        /* Just a wrapper for glfwPollEvents for now */
        window->pollEvents();
        draw();
    }
    teardown();
}