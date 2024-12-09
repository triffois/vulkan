#pragma once

#include "Device.h"
#include "TextureAttachment.h"
#include "TextureData.h"
#include "UniformAttachment.h"
#include <algorithm>
#include <vector>
#include <vulkan/vulkan.h>

using TextureID = int32_t;

class TextureManager {
  public:
    TextureManager(Device *device);
    ~TextureManager() = default;

    TextureManager(const TextureManager &) = delete;
    TextureManager &operator=(const TextureManager &) = delete;

    TextureID registerTexture(const TextureData &textureData);

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

    template <size_t max_n_textures> struct TextureResolutions {
        alignas(16) glm::vec4 resolutions[max_n_textures];
    };

    TextureAttachment getTextureAttachment(uint32_t bindingLocation);
    template <size_t max_n_textures>
    UniformAttachment<TextureResolutions<max_n_textures>>
    getResolutionsAttachment(uint32_t bindingLocation) {
        auto resolutions = getTextureResolutions();
        return UniformAttachment<TextureResolutions<max_n_textures>>(
            device,
            [resolutions = std::move(resolutions)](
                TextureResolutions<max_n_textures> &ubo) {
                // Fill remaining slots with zero
                for (size_t i = resolutions.size(); i < max_n_textures; i++) {
                    ubo.resolutions[i] = glm::vec4(0.0f);
                }
                for (size_t i = 0;
                     i < std::min(resolutions.size(), max_n_textures); i++) {
                    ubo.resolutions[i] = resolutions[i];
                }
            },
            1);
    }

  private:
    Device *device = nullptr;
    std::vector<TextureData> textures;

    static constexpr uint32_t MAX_TEXTURE_COUNT = 256;
    static constexpr uint32_t MAX_TEXTURE_DIMENSION = 1024;

    bool resourcesPrepared = false;
};
