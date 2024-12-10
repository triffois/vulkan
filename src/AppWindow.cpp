#include "AppWindow.h"
#include "Engine.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

AppWindow::~AppWindow() {
    vkDestroySurfaceKHR(*appInstance->getInstance(), surface, nullptr);
    glfwDestroyWindow(window);
}

void AppWindow::init(AppInstance *appInstance,
                     ResizeCallback windowResizeCallback) {
    initializeWindow(appInstance, windowResizeCallback);
    createSurface(appInstance);
    this->appInstance = appInstance;
}

void AppWindow::initializeWindow(AppInstance *appInstance,
                                 ResizeCallback windowResizeCallback) {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window =
        glfwCreateWindow(WINDOWWIDTH, WINDOWHEIGHT, "Vulkan", nullptr, nullptr);
    glfwSetWindowUserPointer(window, appInstance);
    glfwSetFramebufferSizeCallback(window, windowResizeCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(window, WINDOWWIDTH / 2.0, WINDOWHEIGHT / 2.0);
}

void AppWindow::createSurface(const AppInstance *appInstance) {
    if (auto result = glfwCreateWindowSurface(*(appInstance->getInstance()),
                                              window, nullptr, &surface);
        result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface!");
    }
}

GLFWwindow *AppWindow::getWindow() const { return window; }

VkSurfaceKHR AppWindow::getTargetSurface() const { return surface; }
