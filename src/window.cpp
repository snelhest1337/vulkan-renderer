#include <iostream>

#include "common.hpp"
#include "window.hpp"

Window::Window(const WindowArgs &args):
    width(args.width),
    height(args.height),
    name(args.name) {
}

void Window::init() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    /* TODO: Register callback to renderer to signal a resize */
    glfwSetFramebufferSizeCallback(window, nullptr);
}

bool Window::shouldClose() {
    return glfwWindowShouldClose(window);
}

/* Hands out a surface. Caller responsible for destruction */
VkSurfaceKHR Window::createSurface(VkInstance instance) {
    VkSurfaceKHR surface;
    vkc(glfwCreateWindowSurface(instance, window, nullptr, &surface));
    return surface;
}

/* just put everything glfw-related in the window class and think about it later */
void Window::pollEvents() {
    glfwPollEvents();
}

void Window::destroy() {
    glfwDestroyWindow(window);
    glfwTerminate();
}


void Window::getSize(int &width, int &height) {
    glfwGetFramebufferSize(window, &width, &height);
}