#include <iostream>
#include <math.h>
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

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
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = platform.device->getPhysicalDevice();
    allocatorInfo.device = platform.device->get();
    allocatorInfo.instance = platform.instance;
    allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    vmaCreateAllocator(&allocatorInfo, &allocator);
    mainDeletionQueue.push_function([&]() {
        vmaDestroyAllocator(allocator);
    });

    /* Should get its own function */
    int width;
    int height;
    window->getSize(width, height);
    VkExtent3D drawImageExtent = {
        width,
        height,
        1
    };

    drawImage.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
    drawImage.imageExtent = drawImageExtent;


    VkImageUsageFlags drawImageUsages{};
    drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    auto rimgInfo = vkinit::imageCreateInfo(drawImage.imageFormat, drawImageUsages, drawImageExtent);
    VmaAllocationCreateInfo rimgAllocinfo = {};
    rimgAllocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    rimgAllocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vmaCreateImage(allocator, &rimgInfo, &rimgAllocinfo, &drawImage.image, &drawImage.allocation, nullptr);

    auto rviewInfo = vkinit::imageViewCreateInfo(drawImage.imageFormat, drawImage.image, VK_IMAGE_ASPECT_COLOR_BIT);
    vkc(vkCreateImageView(platform.device->get(), &rviewInfo, nullptr, &drawImage.imageView));

    mainDeletionQueue.push_function([&]() {
        vkDestroyImageView(platform.device->get(), drawImage.imageView, nullptr);
        vmaDestroyImage(allocator, drawImage.image, drawImage.allocation);
    });
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
    mainDeletionQueue.flush();
    for (int i = 0; i < FRAME_OVERLAP; i++) {
        frames[i].destroy(platform.device);
    }
    platform.destroy();
    window->destroy();
}

void VulkanApp::drawBackground(VkCommandBuffer cmd) {
    VkClearColorValue clearValue;
    float flash = abs(sin(frameNumber / 120.f));
    clearValue = { { 0.0f, 0.0f, flash, 1.0f } };
    VkImageSubresourceRange clearRange = vkinit::imageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

    vkCmdClearColorImage(cmd, drawImage.image, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);
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

    drawExtent.width = drawImage.imageExtent.width;
    drawExtent.height = drawImage.imageExtent.height;

    vkc(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

    vkutil::transitionImage(cmd, drawImage.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

    drawBackground(cmd);
    vkutil::transitionImage(cmd, drawImage.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    vkutil::transitionImage(cmd, swapchainImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    vkutil::copyImageToImage(cmd, drawImage.image, swapchainImage, drawExtent, platform.swapChain->getExtent());
    vkutil::transitionImage(cmd, swapchainImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
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
    vkc(vkWaitForFences(platform.device->get(), 1, &frame.renderFence, true, SECOND));
    frame.deletionQueue.flush();
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