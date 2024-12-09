#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include "INeedCleanUp.h"

class AppInstance : public INeedCleanUp {

  public:
    AppInstance() = default;
    ~AppInstance() = default;
    void init();

    AppInstance(const AppInstance &) = delete;
    AppInstance(AppInstance &&) = delete;

    AppInstance &operator=(const AppInstance &) = delete;
    AppInstance &operator=(AppInstance &&) = delete;

    const VkInstance *getInstance() const;

    void setAppDevice(const VkDevice *appDevice);
    void setAppWindow(const GLFWwindow *window);

    void cleanUpAll();
    void addComponentToCleanUp(INeedCleanUp *componentToCleanUp,
                               int order = -1);
    void cleanUp(const AppContext &context) override;

  private:
    static VKAPI_ATTR VkBool32 VKAPI_CALL
    debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                  VkDebugUtilsMessageTypeFlagsEXT messageType,
                  const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                  void *pUserData) {
        std::cerr << "validation layer info: " << pCallbackData->pMessage
                  << std::endl;

        return VK_FALSE;
    }

    bool checkValidationLayerSupport();
    std::vector<const char *> getRequiredExtensions();

    void setupDebugMessenger();
    void populateDebugMessengerCreateInfo(
        VkDebugUtilsMessengerCreateInfoEXT &createInfo);

    void createInstance();

  private:
    VkInstance instance;
    const VkDevice *appDevice{};
    const GLFWwindow *appWindow{};

    VkDebugUtilsMessengerEXT debugMessenger;
    std::vector<INeedCleanUp *> componentsToCleanUp;
};
