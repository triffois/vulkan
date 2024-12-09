#pragma once

#include "IAttachment.h"
#include "DescriptorSet.h"
#include "Device.h"
#include "Image.h"
#include "TextureData.h"
#include <memory>

class TextureAttachment : public IAttachment {
  public:
    void init(Device *device, uint32_t max_texture_dimension,
              std::vector<TextureData> &textures);
    void cleanup();

    void updateDescriptorSet(uint32_t maxFramesInFlight, DescriptorSet &descriptorSet) override;
    void update(uint32_t frameIndex) override;

  private:
    Device *device = nullptr;

    std::unique_ptr<Image> textureImage;
    VkSampler textureSampler = VK_NULL_HANDLE;

    void createTextureArray(uint32_t max_texture_dimension,
                            std::vector<TextureData> &textures);
    void updateTextureArray(std::vector<TextureData> &textures);
    void createSampler();
};
