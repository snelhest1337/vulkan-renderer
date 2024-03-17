#include <iostream>
#include <math.h>
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include "app.hpp"
#include "initializers.hpp"
#include "pipeline.hpp"
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
    initDescriptors();
    initPipelines();
}

void VulkanApp::initFrameData() {
    for (int i = 0; i < FRAME_OVERLAP; i++) {
        FrameData frame;
        frame.init(platform.device);
        frames.push_back(frame);
    }
}

void VulkanApp::initDescriptors() {
    std::vector<DescriptorAllocator::PoolSizeRatio> sizes =
    {
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 }
    };
    /* Todo: does GPU support 10 sets? Query device */
    globalDescriptorAllocator.initPool(platform.device->get(), 10, sizes);
    {
        DescriptorLayoutBuilder builder;
        builder.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
        drawImageDescriptorLayout = builder.build(platform.device->get(), VK_SHADER_STAGE_COMPUTE_BIT);
    }

    drawImageDescriptors = globalDescriptorAllocator.allocate(platform.device->get(), drawImageDescriptorLayout);

    VkDescriptorImageInfo imgInfo{};
    imgInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    imgInfo.imageView = drawImage.imageView;

    VkWriteDescriptorSet drawImageWrite{};
    drawImageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    drawImageWrite.pNext = nullptr;

    drawImageWrite.dstBinding = 0;
    drawImageWrite.dstSet = drawImageDescriptors;
    drawImageWrite.descriptorCount = 1;
    drawImageWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    drawImageWrite.pImageInfo = &imgInfo;

    vkUpdateDescriptorSets(platform.device->get(), 1, &drawImageWrite, 0, nullptr);
    mainDeletionQueue.push_function([&]() {
        vkDestroyDescriptorSetLayout(platform.device->get(), drawImageDescriptorLayout, nullptr);
        globalDescriptorAllocator.destroyPool(platform.device->get());
    });
}

void VulkanApp::initPipelines() {
    initBackgroundPipelines();
}

void VulkanApp::initBackgroundPipelines() {
    VkPipelineLayoutCreateInfo computeLayout{};
    computeLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    computeLayout.pNext = nullptr;
    computeLayout.pSetLayouts = &drawImageDescriptorLayout;
    computeLayout.setLayoutCount = 1;

    vkc(vkCreatePipelineLayout(platform.device->get(), &computeLayout, nullptr, &gradientPipelineLayout));
    VkShaderModule computeDrawShader;
    if (!vkutil::loadShaderModule("shaders/gradient.comp.spv", platform.device->get(), &computeDrawShader))
    {
        throw std::runtime_error("Error when building the compute shader");
    }

    VkPipelineShaderStageCreateInfo stageinfo{};
    stageinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stageinfo.pNext = nullptr;
    stageinfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    stageinfo.module = computeDrawShader;
    stageinfo.pName = "main";

    VkComputePipelineCreateInfo computePipelineCreateInfo{};
    computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    computePipelineCreateInfo.pNext = nullptr;
    computePipelineCreateInfo.layout = gradientPipelineLayout;
    computePipelineCreateInfo.stage = stageinfo;

    vkc(vkCreateComputePipelines(platform.device->get(), VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &gradientPipeline));
    vkDestroyShaderModule(platform.device->get(), computeDrawShader, nullptr);
    mainDeletionQueue.push_function([&]() {
        vkDestroyPipelineLayout(platform.device->get(), gradientPipelineLayout, nullptr);
        vkDestroyPipeline(platform.device->get(), gradientPipeline, nullptr);
    });
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
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, gradientPipeline);
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, gradientPipelineLayout, 0, 1, &drawImageDescriptors, 0, nullptr);
    /* Work group size */
    vkCmdDispatch(cmd, std::ceil(drawExtent.width / 16.0), std::ceil(drawExtent.height / 16.0), 1);
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