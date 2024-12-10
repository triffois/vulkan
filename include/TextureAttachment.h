#pragma once

#include "DescriptorSet.h"
#include "Device.h"
#include "IAttachment.h"
#include "Image.h"
#include "TextureData.h"
#include <memory>

class TextureAttachment : public IAttachment {
  public:
    TextureAttachment(Device *device, uint32_t max_texture_dimension,
                      std::vector<TextureData> &textures,
                      uint32_t bindingLocation);

    ~TextureAttachment();

    void cleanUp();

    VkDescriptorSetLayoutBinding layoutBinding() const override;

    void updateDescriptorSet(uint32_t maxFramesInFlight,
                             DescriptorSet &descriptorSet) override;

    void update(uint32_t frameIndex) override;

    VkDescriptorType getType() const override {
        return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    }

  private:
    Device *device;
    uint32_t bindingLocation;

    std::unique_ptr<Image> textureImage;
    VkSampler textureSampler;

    void createTextureArray(uint32_t max_texture_dimension,
                            std::vector<TextureData> &textures);

    void updateTextureArray(std::vector<TextureData> &textures);

    void createSampler();
};
