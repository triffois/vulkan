#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "INeedCleanUp.h"
#include <iostream>
#include <vector>

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class AppInstance : public INeedCleanUp {

public:
    AppInstance() = default;
    ~AppInstance() = default;
    void init();

    AppInstance(const AppInstance& ) = delete;
    AppInstance(AppInstance&& ) = delete;

    AppInstance& operator=(const AppInstance& ) = delete;
    AppInstance& operator=(AppInstance&& ) = delete;

    const VkInstance *getInstance() const;
    void setAppDevice(VkDevice *appDevice);
    void cleanUpAll();
    void addComponentToCleanUp(INeedCleanUp *componentToCleanUp, int order = -1);
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