#pragma once

#include <GLFW/glfw3.h>

#include "EngineTime.h"

class EnginePeripheralsManager {
  public:
    void updatePeripheralsOnFrame() { updateTime(); };

  private:
    void updateTime() {
        Time::currentDeltaTime = glfwGetTime() - Time::prevTimePoint;
        Time::prevTimePoint = glfwGetTime();
    };
};
