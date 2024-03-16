#pragma once
#include <memory>

#include "window.hpp"
#include "platform.hpp"

struct FrameData {
    VkCommandPool commandPool;
    VkCommandBuffer mainCommandBuffer;
};

constexpr unsigned int FRAME_OVERLAP = 2;

/* TODO: Should be broken into main app and renderer. Do later when split is more clear */
class VulkanApp {
    public:
        VulkanApp(std::string name);
        void run();
        void teardown();
        FrameData& getGurrentFrame() { return frames[frameNumber % FRAME_OVERLAP]; };
    private:
        void mainLoop();
        void initCommands();
        std::string name;
        std::shared_ptr<Window> window;
        Platform platform;
        FrameData frames[FRAME_OVERLAP];
        unsigned int frameNumber;
};