#include <iostream>

#include "common.hpp"
#include "framedata.hpp"
#include "initializers.hpp"

void FrameData::init(std::shared_ptr<Device> device) {
    auto commandPoolInfo = vkinit::commandPoolCreateInfo(device->getGraphicsFamilyIndex());
    vkc(vkCreateCommandPool(device->get(), &commandPoolInfo, nullptr, &commandPool));

    auto cmdAllocInfo = vkinit::commandBufferAllocInfo(commandPool, 1);
    vkc(vkAllocateCommandBuffers(device->get(), &cmdAllocInfo, &mainCommandBuffer));

    auto fenceInfo = vkinit::fenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
    auto semaphoreInfo = vkinit::semaphoreCreateInfo(0);
    vkc(vkCreateFence(device->get(), &fenceInfo, nullptr, &renderFence));
    vkc(vkCreateSemaphore(device->get(), &semaphoreInfo, nullptr, &renderSemaphore));
    vkc(vkCreateSemaphore(device->get(), &semaphoreInfo, nullptr, &swapchainSemaphore));

}

void FrameData::destroy(std::shared_ptr<Device> device) {
    vkDestroyCommandPool(device->get(), commandPool, nullptr);
    vkDestroySemaphore(device->get(), renderSemaphore, nullptr);
    vkDestroySemaphore(device->get(), swapchainSemaphore, nullptr);
    vkDestroyFence(device->get(), renderFence, nullptr);
}