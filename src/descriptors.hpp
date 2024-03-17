#pragma once
/* TODO: this should not have a glfw dependency */
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <span>

struct DescriptorLayoutBuilder {
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    void addBinding(uint32_t binding, VkDescriptorType type);
    void clear();
    VkDescriptorSetLayout build(VkDevice device, VkShaderStageFlags shaderStages);
};


struct DescriptorAllocator {
    struct PoolSizeRatio {
        VkDescriptorType type;
        float ratio;
    };

    VkDescriptorPool pool;
    void initPool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios);
    void clearDescriptors(VkDevice device);
    void destroyPool(VkDevice device);
    VkDescriptorSet allocate(VkDevice device, VkDescriptorSetLayout layout);

};