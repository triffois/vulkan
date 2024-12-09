#pragma once

#include "DescriptorSet.h"
#include "Device.h"
#include "Image.h"
#include "TextureData.h"
#include <memory>

class TextureAttachment {
  public:
    void init(Device *device, uint32_t max_texture_dimension,
              std::vector<TextureData> &textures);
    void cleanup();

    VkSampler getSampler() const { return textureSampler; }
    VkImageView getTextureArrayView() const {
        return textureImage->getVkImageView();
    }

    void updateDescriptorSet(uint32_t maxFramesInFlight,
                             DescriptorSet &descriptorSet);

  private:
    Device *device = nullptr;

    std::unique_ptr<Image> textureImage;
    VkSampler textureSampler = VK_NULL_HANDLE;

    void createTextureArray(uint32_t max_texture_dimension,
                            std::vector<TextureData> &textures);
    void updateTextureArray(std::vector<TextureData> &textures);
    void createSampler();
};
