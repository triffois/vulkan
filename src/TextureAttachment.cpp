#include "TextureAttachment.h"
#include "Buffer.h"

void TextureAttachment::init(Device *device, uint32_t max_texture_dimension,
                             std::vector<TextureData> &textures) {
    this->device = device;
    textureImage = std::make_unique<Image>(*device);

    createTextureArray(max_texture_dimension, textures);
    createSampler();
}

void TextureAttachment::cleanup() {
    if (device) {
        if (textureSampler != VK_NULL_HANDLE) {
            vkDestroySampler(*device->getDevice(), textureSampler, nullptr);
            textureSampler = VK_NULL_HANDLE;
        }
        textureImage->cleanUp();
    }
}

void TextureAttachment::createTextureArray(uint32_t max_texture_dimension,
                                           std::vector<TextureData> &textures) {
    textureImage->createImage(
        max_texture_dimension, max_texture_dimension, VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VMA_MEMORY_USAGE_GPU_ONLY,
        VMA_ALLOCATION_CREATE_STRATEGY_MIN_TIME_BIT, textures.size());

    textureImage->createImageView(VK_FORMAT_R8G8B8A8_SRGB,
                                  VK_IMAGE_ASPECT_COLOR_BIT,
                                  VK_IMAGE_VIEW_TYPE_2D_ARRAY, textures.size());

    updateTextureArray(textures);
}

void TextureAttachment::updateTextureArray(std::vector<TextureData> &textures) {
    if (textures.empty()) {
        return;
    }

    VkDeviceSize totalSize = 0;
    for (const auto &texture : textures) {
        totalSize += texture.width * texture.height * 4;
    }

    Buffer stagingBuffer(
        device, totalSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        VMA_MEMORY_USAGE_AUTO,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

    char *data;
    stagingBuffer.map(reinterpret_cast<void **>(&data));

    VkDeviceSize offset = 0;
    std::vector<VkBufferImageCopy> copyRegions;

    for (size_t i = 0; i < textures.size(); i++) {
        const auto &texture = textures[i];
        VkDeviceSize imageSize = texture.width * texture.height * 4;

        memcpy(data + offset, texture.pixels.data(), imageSize);

        VkBufferImageCopy copyRegion{};
        copyRegion.bufferOffset = offset;
        copyRegion.bufferRowLength = 0;
        copyRegion.bufferImageHeight = 0;
        copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegion.imageSubresource.mipLevel = 0;
        copyRegion.imageSubresource.baseArrayLayer = i;
        copyRegion.imageSubresource.layerCount = 1;
        copyRegion.imageOffset = {0, 0, 0};
        copyRegion.imageExtent = {static_cast<uint32_t>(texture.width),
                                  static_cast<uint32_t>(texture.height), 1};

        copyRegions.push_back(copyRegion);
        offset += imageSize;
    }

    stagingBuffer.unmap();

    textureImage->transitionImageLayout(VK_FORMAT_R8G8B8A8_SRGB,
                                        VK_IMAGE_LAYOUT_UNDEFINED,
                                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    stagingBuffer.copyToImage(textureImage->getVkImage(), copyRegions);

    textureImage->transitionImageLayout(
        VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void TextureAttachment::createSampler() {
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = 16.0f;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    if (vkCreateSampler(*device->getDevice(), &samplerInfo, nullptr,
                        &textureSampler) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create texture sampler");
    }
}

void TextureAttachment::updateDescriptorSet(uint32_t maxFramesInFlight,
                                            DescriptorSet &descriptorSet) {
    descriptorSet.updateImageInfo(1, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                  textureImage->getVkImageView(),
                                  textureSampler);
}

void TextureAttachment::update(uint32_t frameIndex) {}
