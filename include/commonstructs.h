#pragma once

#include "vulkan/vulkan_core.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <array>
#include <optional>
#include <vector>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    [[nodiscard]] bool isComplete() const {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

enum class VertexAttributesLocations : size_t {
    POSITION_LOCATION = 0,
    COLOR_LOCATION,
    TEXTURE_COORDINATE_LOCATION,
    NORMAL_VECTOR_LOCATION,
    INSTANCE_MATRIX_START_LOCATION = 4,
    INSTANCE_MATERIAL_LOCATION = 8,
};

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
    glm::vec3 normalVector;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 4>
    getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 4>
                attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location =
                static_cast<size_t>(VertexAttributesLocations::POSITION_LOCATION);
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location =
                static_cast<size_t>(VertexAttributesLocations::COLOR_LOCATION);
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = static_cast<size_t>(
            VertexAttributesLocations::TEXTURE_COORDINATE_LOCATION);
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = static_cast<size_t>(
            VertexAttributesLocations::NORMAL_VECTOR_LOCATION);
        attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[3].offset = offsetof(Vertex, normalVector);

        return attributeDescriptions;
    }
};

struct UniformBufferObject {
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
    alignas(16) glm::vec4 camPos{};
};

struct PointLightSource {
    alignas(16) glm::vec4 lightColor{};
    alignas(16) glm::vec4 lightPos{};
    alignas(4) float lightIntensity{};
    alignas(4) int lightRange{};
};

struct DirectionalLightSource {
    alignas(16) glm::vec4 lightDirection;
    alignas(16) glm::vec4 lightColor;
};

struct SceneLightData {
    alignas(16) PointLightSource pointLights[3];
    alignas(16) DirectionalLightSource dirLight;
};

struct AppContext {
    const VkInstance *appInstance;
    const VkDevice *appDevice;
    const GLFWwindow *appWindow;
};
