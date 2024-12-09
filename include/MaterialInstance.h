#pragma once

#include <array>
#include <cstdint>

constexpr std::size_t MAX_TEXTURES_PER_MATERIAL = 4;

struct MaterialInstance {
    std::array<int32_t, MAX_TEXTURES_PER_MATERIAL> textureIds;

    MaterialInstance() : textureIds{-1, -1, -1, -1} {}
};
