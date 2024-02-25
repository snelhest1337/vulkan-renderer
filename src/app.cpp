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
    /* renderer({.device = platform.getDevice()}) */ {
    window->init();
    platform.init({.name = name, .useValidationLayers = true, .window = window});
}

void VulkanApp::teardown() {
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
        break;
    }
    teardown();
}