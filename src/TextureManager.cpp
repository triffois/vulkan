#include "TextureManager.h"
#include <stdexcept>

void TextureManager::init(Device *device) {
    this->device = device;
    textureAttachment.init(device);
}

void TextureManager::cleanup() { textureAttachment.cleanup(); }

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

void TextureManager::prepareResources() {
    if (resourcesPrepared) {
        std::runtime_error("Resources have already been prepared");
    }
    textureAttachment.createTextureArray(MAX_TEXTURE_DIMENSION, textures);
    textureAttachment.createSampler();
    resourcesPrepared = true;
}
