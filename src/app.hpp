#pragma once
#include <memory>

#include <vk_mem_alloc.h>

#include "window.hpp"
#include "platform.hpp"
#include "framedata.hpp"
#include "deletionqueue.hpp"
#include "images.hpp"
#include "descriptors.hpp"

constexpr unsigned int FRAME_OVERLAP = 2;

/* TODO: Should be broken into main app and renderer. Do later when split is more clear */
class VulkanApp {
    public:
        VulkanApp(std::string name);
        ~VulkanApp();
        void init();
        void run();
        void teardown();
        FrameData& getCurrentFrame() {
            return frames[frameNumber % FRAME_OVERLAP];
        };
        DescriptorAllocator globalDescriptorAllocator;
        VkDescriptorSet drawImageDescriptors;
        VkDescriptorSetLayout drawImageDescriptorLayout;
        /*
         * Should be something like a hashmap with type -> pipeline lookup
         * Perhaps its own object for pipeline management.
         */
        VkPipeline gradientPipeline;
        VkPipelineLayout gradientPipelineLayout;
    private:
        void mainLoop();
        void initFrameData();
        void initDescriptors();
        void initPipelines();
        void initBackgroundPipelines();
        void draw();
        void drawBackground(VkCommandBuffer cmd);
        std::string name;
        std::vector<FrameData> frames;
        std::shared_ptr<Window> window;
        Platform platform;
        unsigned int frameNumber;
        DeletionQueue mainDeletionQueue;
        /* Would be fun to make my own sometime */
        VmaAllocator allocator;
        AllocatedImage drawImage;
        VkExtent2D drawExtent;
};