#pragma once

#include <array>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

// This struct repreents the GPU-side instance data layout
struct InstanceData {
    alignas(16) glm::mat4 transform;
    alignas(16) glm::vec4 textureIndices; // Packed texture indices

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 1; // Use binding point 1 for instance data
        bindingDescription.stride = sizeof(InstanceData);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 5>
    getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 5>
            attributeDescriptions{};

        // Transform matrix (4 vec4s)
        for (uint32_t i = 0; i < 4; i++) {
            attributeDescriptions[i].binding = 1;
            attributeDescriptions[i].location = 3 + i; // Locations 3-6
            attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[i].offset = i * sizeof(glm::vec4);
        }

        // Material data (texture indices)
        attributeDescriptions[4].binding = 1;
        attributeDescriptions[4].location = 7; // Location 7
        attributeDescriptions[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[4].offset =
            offsetof(InstanceData, textureIndices);

        return attributeDescriptions;
    }
};
