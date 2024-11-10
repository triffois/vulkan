#include "AppWindow.h"
#include "Engine.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

void AppWindow::init(AppInstance *appInstance, ResizeCallback windowResizeCallback){
    initializeWindow(appInstance, windowResizeCallback);
    createSurface(appInstance);
}

static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    static float lastCursorPosX{WINDOWWIDTH / 2.0};
    static float lastCursorPosY{WINDOWHEIGHT / 2.0};

    float dX = xpos - lastCursorPosX;
    float dY = ypos - lastCursorPosY;
    lastCursorPosX = xpos;
    lastCursorPosY = ypos;

    auto app = reinterpret_cast<Engine*>(glfwGetWindowUserPointer(window));
    app->getCamera()->ProcessMouseMovement(dX, dY);
}

static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    auto app = reinterpret_cast<Engine*>(glfwGetWindowUserPointer(window));
    app->getCamera()->ProcessMouseScroll(yoffset);
}

void AppWindow::cleanUp(const CleanUpContex &context) {
    vkDestroySurfaceKHR(*context.appInstance, surface, nullptr);
    glfwDestroyWindow(window);
}

void AppWindow::initializeWindow(AppInstance *appInstance, ResizeCallback windowResizeCallback) {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(WINDOWWIDTH,WINDOWHEIGHT, "Vulkan", nullptr, nullptr);
    glfwSetWindowUserPointer(window, appInstance);
    glfwSetFramebufferSizeCallback(window, windowResizeCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, scrollCallback);
}

void AppWindow::createSurface(const AppInstance *appInstance) {
    if (auto result = glfwCreateWindowSurface(*(appInstance->getInstance()), window, nullptr, &surface); result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface!");
    }
}

GLFWwindow *AppWindow::getWindow() {
    return window;
}

VkSurfaceKHR* AppWindow::getTargetSurface() {
    return &surface;
}
