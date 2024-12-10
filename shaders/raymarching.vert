#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    vec4 camPos;
}
ubo;

// vertex in
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormalVector;

// instance in
layout(location = 4) in mat4 inModel;
layout(location = 8) in ivec4 inMaterial;

// out data
layout(location = 0) out vec3 rayOrigin;
layout(location = 1) out vec3 nearPos;

void main() {
    // Transform the vertex position using model matrix
    vec4 worldPos = inModel * vec4(inPosition, 1.0);
    gl_Position = ubo.proj * ubo.view * worldPos;

    // Camera position remains the same
    rayOrigin = ubo.camPos.xyz;

    // Calculate world space position for this vertex
    nearPos = worldPos.xyz;
}
