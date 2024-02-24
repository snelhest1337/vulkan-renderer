#include <iostream>

#include "app.hpp"

VulkanApp::VulkanApp(std::string name):
    name(name),
    platform({.name = name, .useValidationLayers = false}) {

}

void VulkanApp::run() {
    mainLoop();
}

void VulkanApp::mainLoop() {

}