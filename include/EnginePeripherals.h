#pragma once

#include <GLFW/glfw3.h>

#include "Time.h"

class EnginePeripheralsManager {
  public:
    void updatePeripheralsOnFrame() { updateTime(); };

  private:
    void updateTime() {
        Time::currentDeltaTime = glfwGetTime() - Time::prevTimePoint;
        Time::prevTimePoint = glfwGetTime();
    };
};
