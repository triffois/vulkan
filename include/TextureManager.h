#pragma once

#include "Device.h"
#include "Image.h"
#include "TextureData.h"
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

using TextureID = int32_t;

class TextureManager {
  public:
    TextureManager() = default;
    ~TextureManager() = default;

    TextureManager(const TextureManager &) = delete;
    TextureManager &operator=(const TextureManager &) = delete;

    TextureID registerTexture(const TextureData &textureData);

    VkSampler getSampler() const { return textureSampler; }
    VkImageView getTextureArrayView() const {
        return textureImage->getVkImageView();
    }
    void init(Device *device);
    void prepareResources();
    void cleanup();

    std::vector<glm::vec4> getTextureResolutions() const {
        std::vector<glm::vec4> resolutions;
        resolutions.reserve(textures.size());
        for (size_t i = 0; i < textures.size(); i++) {
            const auto &texture = textures[i];
            auto resolution = glm::vec4(
                texture.width / static_cast<float>(MAX_TEXTURE_DIMENSION),
                texture.height / static_cast<float>(MAX_TEXTURE_DIMENSION), 0,
                0);
            resolutions.push_back(resolution);
        }
        return resolutions;
    }

  private:
    void createTextureArray();
    void updateTextureArray();
    void createSampler();

    Device *device = nullptr;
    std::vector<TextureData> textures;

    std::unique_ptr<Image> textureImage;
    VkSampler textureSampler = VK_NULL_HANDLE;

    static constexpr uint32_t MAX_TEXTURE_COUNT = 256;
    static constexpr uint32_t MAX_TEXTURE_DIMENSION = 1024;

    bool resourcesPrepared = false;
};