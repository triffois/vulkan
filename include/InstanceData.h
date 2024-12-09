#pragma once

#include "commonstructs.h"

#include <array>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

// This struct repreents the GPU-side instance data layout
struct InstanceData {
    alignas(16) glm::mat4 transform;
    alignas(
        16) std::array<int32_t, 4> textureIndices; // Array of texture indices

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

        auto matrixStartLocation = static_cast<size_t>(
            VertexAttributesLocations::INSTANCE_MATRIX_START_LOCATION);
        // Transform matrix (4 vec4s)
        for (uint32_t i = 0; i < 4; i++) {
            attributeDescriptions[i].binding = 1;
            attributeDescriptions[i].location =
                matrixStartLocation + i; // Locations 3-6
            attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[i].offset = i * sizeof(glm::vec4);
        }

        // Material data (texture indices)
        attributeDescriptions[4].binding = 1;
        attributeDescriptions[4].location = static_cast<size_t>(
            VertexAttributesLocations::INSTANCE_MATERIAL_LOCATION); // Location
                                                                    // 7
        attributeDescriptions[4].format = VK_FORMAT_R32G32B32A32_SINT;
        attributeDescriptions[4].offset =
            offsetof(InstanceData, textureIndices);

        return attributeDescriptions;
    }
};
