#pragma once

#include "INeedCleanUp.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <queue>

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

class AppInstance : INeedCleanUp {

public:
    AppInstance();
    ~AppInstance();
    const VkInstance *getInstance();
    void setAppDevice(VkDevice *appDevice);
    void cleanUpAll();
    void addComponentToCleanUp(size_t order, INeedCleanUp *componentToCleanUp);
    void cleanUp(const CleanUpContex &context) override;

private:

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
        std::cerr << "validation layer info: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

    bool checkValidationLayerSupport();
    std::vector<const char*> getRequiredExtensions();

    void setupDebugMessenger();
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    void createInstance();

private:
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkDevice *appDevice{};

     std::vector<INeedCleanUp *> componentsToCleanUp;
};