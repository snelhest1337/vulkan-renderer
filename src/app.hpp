#pragma once
#include <memory>

#include "window.hpp"
#include "platform.hpp"
#include "framedata.hpp"

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
    private:
        void mainLoop();
        void initFrameData();
        void draw();
        std::string name;
        std::vector<FrameData> frames;
        std::shared_ptr<Window> window;
        Platform platform;
        unsigned int frameNumber;
};