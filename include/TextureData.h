#pragma once

#include <string>
#include <vector>

struct TextureData {
    std::vector<unsigned char> pixels;
    int width;
    int height;
    int channels;
    std::string mimeType;
};
