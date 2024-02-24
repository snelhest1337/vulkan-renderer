#include "window.hpp"

Window::Window(const WindowArgs &args):
    width(args.width),
    height(args.height),
    name(args.name) {
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

/* just put everything glfw-related in the window class and think about it later */
void Window::pollEvents() {
    glfwPollEvents();
}

Window::~Window() {
    glfwDestroyWindow(window);
    glfwTerminate();
}