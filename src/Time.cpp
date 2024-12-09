#include "Time.h"
#include <GLFW/glfw3.h>

double Time::prevTimePoint = glfwGetTime();
double Time::currentDeltaTime = 0;