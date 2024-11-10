 #pragma once

#include "INeedCleanUp.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class AppWindow : INeedCleanUp {

public:
    AppWindow();
    ~AppWindow();
    GLFWwindow *getWindow();
    VkSurfaceKHR *getTargetSurface();

private:
    GLFWwindow* window;
    VkSurfaceKHR surface;
};