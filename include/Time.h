#pragma once
#include <GLFW/glfw3.h>

class EnginePeripheralsManager;
class Time
{
  friend class EnginePeripheralsManager;

  public:
    static double deltaTime(){return currentDeltaTime;};
    static double currentTime(){return prevTimePoint;};

  private:
    static double prevTimePoint;
    static double currentDeltaTime;
};

double Time::prevTimePoint = glfwGetTime();
double Time::currentDeltaTime = 0;
