#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    vec2 textureResolutions[256];
}
ubo;

layout(binding = 1) uniform sampler2DArray texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) flat in ivec4 material;

layout(location = 0) out vec4 outColor;

void main() {
    int textureIndex = material.x;

    if (textureIndex < 0) {
        outColor = vec4(fragColor, 1.0);
    } else {
        vec2 resolution = ubo.textureResolutions[textureIndex];
        vec2 scaledUV = fragTexCoord * resolution;
        outColor = texture(texSampler, vec3(scaledUV, float(textureIndex)));
    }
}
