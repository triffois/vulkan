#pragma once

#include "Buffer.h"
#include "Camera.h"
#include "DescriptorLayout.h"
#include "DescriptorPool.h"
#include "DescriptorSet.h"
#include "Device.h"
#include "Image.h"
#include "Model.h"
#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

class Pipeline {
  public:
    Pipeline(Device *device, const std::string &vertShaderPath,
             const std::string &fragShaderPath, VkFormat colorFormat,
             VkFormat depthFormat, const Model &model,
             uint32_t maxFramesInFlight, const std::vector<PerInstanceData> &instanceData = {});

    // TODO think about how to handle copying

    void cleanup();
    void updateUniformBuffer(uint32_t currentFrame, Camera &camera,
                             const VkExtent2D &swapChainExtent) const;

    void updateUniformLightingBuffers() const; //TODO: this will be more needed when the lighting becomes dynamic

    VkPipeline getPipeline() const { return graphicsPipeline; }
    VkPipelineLayout getLayout() const { return pipelineLayout; }
    const Model &getModel() const { return model; }

    VkDescriptorSet getDescriptorSet(uint32_t frameIndex) const {
        return descriptorSet.getSet(frameIndex);
    }
    VkBuffer getVertexBuffer() const { return vertexBuffer->getBuffer(); }
    VkBuffer getIndexBuffer() const { return indexBuffer->getBuffer(); }
    VkBuffer getInstanceDataBuffer() const {return instanceDataBuffer->getBuffer();};
    uint32_t getIndexCount() const { return model.getIndices().size(); }

    bool isInstancedRenderingUsed() const {return ifUseInstancedRendering;};
    size_t getNumInstances() const {return numInstances;};

    glm::mat4 modelMatrix = glm::mat4(1.0f);

  private:
    Device *device;
    VkPipeline graphicsPipeline;
    VkPipelineLayout pipelineLayout;
    Model model;
    std::vector<std::unique_ptr<Image>> images;

    VkSampler textureSampler;

    std::unique_ptr<Buffer> vertexBuffer;
    std::unique_ptr<Buffer> indexBuffer;
    std::unique_ptr<Buffer> instanceDataBuffer;

    DeviceMemoryAllocationHandle vertexBufferAllocation;
    DeviceMemoryAllocationHandle indexBufferAllocation;

    bool ifUseInstancedRendering{false};
    size_t numInstances{0};

    void createVertexBuffer();
    void createIndexBuffer();
    void createInstanceDataBuffer(const std::vector<PerInstanceData> &instanceData);

    DeviceMemoryAllocationHandle createBuffer(VkDeviceSize size,
                                              VkBufferUsageFlags usage,
                                              VkMemoryPropertyFlags properties,
                                              VkBuffer &buffer);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    DescriptorLayout descriptorLayout;
    DescriptorPool descriptorPool;
    DescriptorSet descriptorSet;

    std::vector<std::unique_ptr<Buffer>> uniformBuffers;
    std::vector<void *> uniformBuffersMapped;
    std::vector<DeviceMemoryAllocationHandle> uniformBuffersAllocations;

    std::vector<std::unique_ptr<Buffer>> uniformLightingBuffers;
    std::vector<void *> mappedUniformLightingBuffers;
    std::vector<DeviceMemoryAllocationHandle> uniformLightsAllocations;
    const size_t numStaticLightSources;

    glm::mat4 calculateModelMatrix() const;

    void createUniformBuffers(uint32_t maxFramesInFlight);
    VkShaderModule createShaderModule(const std::vector<char> &code);
    std::vector<char> readFile(const std::string &filename);

    void createUniformStaticLightingBuffers();

    void createTextureResources();
    void createTextureSampler();
};
