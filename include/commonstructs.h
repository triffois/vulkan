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
    INSTANCE_MAT_COLUMN_0,
    INSTANCE_MAT_COLUMN_1,
    INSTANCE_MAT_COLUMN_2,
    INSTANCE_MAT_COLUMN_3,
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
        attributeDescriptions[0].location = static_cast<size_t>(VertexAttributesLocations::POSITION_LOCATION);
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = static_cast<size_t>(VertexAttributesLocations::COLOR_LOCATION);
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = static_cast<size_t>(VertexAttributesLocations::TEXTURE_COORDINATE_LOCATION);
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = static_cast<size_t>(VertexAttributesLocations::NORMAL_VECTOR_LOCATION);
        attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[3].offset = offsetof(Vertex, normalVector);

        return attributeDescriptions;
    }
};

struct PerInstanceData {
    glm::mat4 instanceWorldTransform;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 1;
        bindingDescription.stride = sizeof(PerInstanceData);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 4>
    getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 4>
            attributeDescriptions{};

        auto step = sizeof(glm::vec4);

        attributeDescriptions[0].binding = 1;
        attributeDescriptions[0].location = static_cast<size_t>(VertexAttributesLocations::INSTANCE_MAT_COLUMN_0);
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[0].offset = 0 * step;

        attributeDescriptions[1].binding = 1;
        attributeDescriptions[1].location = static_cast<size_t>(VertexAttributesLocations::INSTANCE_MAT_COLUMN_1);
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[1].offset = 1 * step;

        attributeDescriptions[2].binding = 1;
        attributeDescriptions[2].location = static_cast<size_t>(VertexAttributesLocations::INSTANCE_MAT_COLUMN_2);
        attributeDescriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[2].offset = 2 * step;

        attributeDescriptions[3].binding = 1;
        attributeDescriptions[3].location = static_cast<size_t>(VertexAttributesLocations::INSTANCE_MAT_COLUMN_3);
        attributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[3].offset = 3 * step;

        return attributeDescriptions;
    }
};

struct SimpleLightSource { //alignas(16)
    alignas(16) glm::vec3 lightColor{};
    alignas(16) glm::vec3 lightPos{};
    alignas(4)  float lightIntensity{};
};

struct UniformBufferObject {
    alignas(16) glm::mat4 model{};
    alignas(16) glm::mat4 view{};
    alignas(16) glm::mat4 proj{};
    alignas(16) SimpleLightSource lights[3]{{},{},{}};
};

struct AppContext {
    const VkInstance *appInstance;
    const VkDevice *appDevice;
    const GLFWwindow *appWindow;
};
