#include "DescriptorPool.h"
#include <stdexcept>
#include <vector>

void DescriptorPool::init(
    VkDevice device,
    std::unordered_map<VkDescriptorType, uint32_t> &descriptorTypeCounts,
    uint32_t maxSets) {
    this->device = device;

    std::vector<VkDescriptorPoolSize> poolSizes;
    for (auto &pair : descriptorTypeCounts) {
        VkDescriptorPoolSize poolSize{};
        poolSize.type = pair.first;
        poolSize.descriptorCount = pair.second * maxSets;
        poolSizes.push_back(poolSize);
    }

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = maxSets;

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void DescriptorPool::cleanup() {
    if (descriptorPool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(device, descriptorPool, nullptr);
        descriptorPool = VK_NULL_HANDLE;
    }
}
