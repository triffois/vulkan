#pragma once

#include "Device.h"
#include "Image.h"
#include "TextureData.h"
#include <memory>

class TextureAttachment {
  public:
    void init(Device *device);
    void cleanup();

    VkSampler getSampler() const { return textureSampler; }
    VkImageView getTextureArrayView() const {
        return textureImage->getVkImageView();
    }

    // TODO private
    void createTextureArray(uint32_t max_texture_dimension,
                            std::vector<TextureData> &textures);
    void updateTextureArray(std::vector<TextureData> &textures);
    void createSampler();

  private:
    Device *device = nullptr;

    std::unique_ptr<Image> textureImage;
    VkSampler textureSampler = VK_NULL_HANDLE;
};
