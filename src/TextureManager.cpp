#include "TextureManager.h"
#include "Buffer.h"
#include <stdexcept>

void TextureManager::init(Device *device) {
    this->device = device;
    textureImage = std::make_unique<Image>(*device);
}

void TextureManager::cleanup() {
    if (device) {
        if (textureSampler != VK_NULL_HANDLE) {
            vkDestroySampler(*device->getDevice(), textureSampler, nullptr);
            textureSampler = VK_NULL_HANDLE;
        }
        textureImage->cleanUp();
    }
}

TextureID TextureManager::registerTexture(const TextureData &textureData) {
    if (textures.size() >= MAX_TEXTURE_COUNT) {
        throw std::runtime_error("Maximum texture count exceeded");
    }

    if (textureData.width > MAX_TEXTURE_DIMENSION ||
        textureData.height > MAX_TEXTURE_DIMENSION) {
        throw std::runtime_error(
            "Texture dimensions exceed maximum allowed size");
    }

    textures.push_back(textureData);
    needsUpdate = true;
    return static_cast<TextureID>(textures.size() - 1);
}

void TextureManager::createTextureArray() {
    if (textures.empty()) {
        return;
    }

    textureImage->createImage(
        MAX_TEXTURE_DIMENSION, MAX_TEXTURE_DIMENSION, VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VMA_MEMORY_USAGE_GPU_ONLY,
        VMA_ALLOCATION_CREATE_STRATEGY_MIN_TIME_BIT, MAX_TEXTURE_COUNT);

    textureImage->createImageView(
        VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT,
        VK_IMAGE_VIEW_TYPE_2D_ARRAY, MAX_TEXTURE_COUNT);

    updateTextureArray();
}

void TextureManager::updateTextureArray() {
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

    needsUpdate = false;
}

void TextureManager::prepareResources() {
    if (!resourcesPrepared) {
        createTextureArray();
        createSampler();
        resourcesPrepared = true;
    } else if (needsUpdate) {
        updateTextureArray();
    }
}

void TextureManager::createSampler() {
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
