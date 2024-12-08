#pragma once

#include "AppInstance.h"
#include "INeedCleanUp.h"

using ResizeCallback = void (*)(GLFWwindow *window, int width, int height);

const uint32_t WINDOWWIDTH = 800;
const uint32_t WINDOWHEIGHT = 600;

class AppWindow : public INeedCleanUp {

  public:
    AppWindow() = default;
    ~AppWindow() = default;
    void init(AppInstance *appInstance, ResizeCallback windowResizeCallback);

    AppWindow(const AppInstance &) = delete;
    AppWindow(AppInstance &&) = delete;

    AppWindow &operator=(const AppWindow &) = delete;
    AppWindow &operator=(AppWindow &&) = delete;

    [[nodiscard]] GLFWwindow *getWindow() const;
    [[nodiscard]] VkSurfaceKHR getTargetSurface() const;
    void cleanUp(const AppContext &context) override;

  private:
    void initializeWindow(AppInstance *appInstance,
                          ResizeCallback windowResizeCallback);
    void createSurface(const AppInstance *appInstance);

  private:
    GLFWwindow *window{};
    VkSurfaceKHR surface;
};
