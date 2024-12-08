#pragma once

class EnginePeripheralsManager;

class Time {
    friend class EnginePeripheralsManager;

  public:
    static double deltaTime() { return currentDeltaTime; };
    static double currentTime() { return prevTimePoint; };

  private:
    static double prevTimePoint;
    static double currentDeltaTime;
};
