#include <iostream>

#include "app.hpp"

#define WIDTH 1280
#define HEIGHT 720

VulkanApp::VulkanApp(std::string name):
    name(name),
    platform({.name = name, .useValidationLayers = true}),
    window({.width = WIDTH, .height = HEIGHT, .name = name}) {

}

void VulkanApp::run() {
    mainLoop();
}

void VulkanApp::mainLoop() {
    while (!window.shouldClose()) {
        /* Just a wrapper for glfwPollEvents for now */
        window.pollEvents();
    }
}