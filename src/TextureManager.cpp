#include "TextureManager.h"
#include <stdexcept>

TextureManager::TextureManager(Device *device) : device(device) {}

TextureID TextureManager::registerTexture(const TextureData &textureData) {
    if (resourcesPrepared) {
        throw std::runtime_error(
            "Cannot register texture after resources have been prepared");
    }
    if (textures.size() >= MAX_TEXTURE_COUNT) {
        throw std::runtime_error("Maximum texture count exceeded");
    }

    if (textureData.width > MAX_TEXTURE_DIMENSION ||
        textureData.height > MAX_TEXTURE_DIMENSION) {
        throw std::runtime_error(
            "Texture dimensions exceed maximum allowed size");
    }

    textures.push_back(textureData);
    return static_cast<TextureID>(textures.size() - 1);
}

TextureAttachment
TextureManager::getTextureAttachment(uint32_t bindingLocation) {
    return TextureAttachment(device, MAX_TEXTURE_DIMENSION, textures,
                             bindingLocation);
}
