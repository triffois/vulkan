#include "DescriptorSet.h"

#include <iostream>
#include <stdexcept>

void DescriptorSet::init(VkDevice device, const DescriptorPool &pool,
                         const DescriptorLayout &layout, uint32_t count) {
    this->device = device;
    descriptorSets.resize(count);

    std::vector<VkDescriptorSetLayout> layouts(count, layout.getLayout());

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = pool.getPool();
    allocInfo.descriptorSetCount = count;
    allocInfo.pSetLayouts = layouts.data();

    if (auto res = vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()); res !=
        VK_SUCCESS) {
        std::cout << res << std::endl;
        throw std::runtime_error("failed to allocate descriptor sets!");
    }
}

void DescriptorSet::updateBufferInfo(uint32_t binding, VkBuffer buffer,
                                     VkDeviceSize offset, VkDeviceSize range) {
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = buffer;
    bufferInfo.offset = offset;
    bufferInfo.range = range;

    for (size_t i = 0; i < descriptorSets.size(); i++) {
        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSets[i];
        descriptorWrite.dstBinding = binding;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
    }
}

void DescriptorSet::updateImageInfos(uint32_t startBinding,
                                     const std::vector<VkImageView> &views,
                                     const std::vector<VkImageLayout> &layouts,
                                     VkSampler sampler) {
    if (views.empty() || views.size() != layouts.size()) {
        return;
    }

    std::vector<VkDescriptorImageInfo> imageInfos(views.size());
    std::vector<VkWriteDescriptorSet> descriptorWrites(views.size());

    // Prepare all image infos
    for (size_t i = 0; i < views.size(); i++) {
        imageInfos[i].imageLayout = layouts[i];
        imageInfos[i].imageView = views[i];
        imageInfos[i].sampler = sampler;
    }

    // Update all descriptor sets
    for (size_t setIdx = 0; setIdx < descriptorSets.size(); setIdx++) {
        for (size_t i = 0; i < views.size(); i++) {
            descriptorWrites[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[i].dstSet = descriptorSets[setIdx];
            descriptorWrites[i].dstBinding = startBinding + i;
            descriptorWrites[i].dstArrayElement = 0;
            descriptorWrites[i].descriptorType =
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[i].descriptorCount = 1;
            descriptorWrites[i].pImageInfo = &imageInfos[i];
        }

        vkUpdateDescriptorSets(device,
                               static_cast<uint32_t>(descriptorWrites.size()),
                               descriptorWrites.data(), 0, nullptr);
    }
}

void DescriptorSet::updateImageInfo(uint32_t binding, VkImageLayout layout,
                                    VkImageView view, VkSampler sampler) {
    // Use the new method with single image
    updateImageInfos(binding, std::vector<VkImageView>{view},
                     std::vector<VkImageLayout>{layout}, sampler);
}

VkDescriptorSet DescriptorSet::getSet(uint32_t index) const {
    return descriptorSets[index];
}
