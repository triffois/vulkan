#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    vec4 camPos;
} ubo;

//vertex in
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormalVector;

//instance in
layout(location = 4) in mat4 inModel;
layout(location = 8) in ivec4 inMaterial;

//out data
layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec4 vertexPos;
layout(location = 3) out vec3 normalVector;

layout(location = 4) flat out ivec4 fragMaterial;

void main() {
    vertexPos = inModel * vec4(inPosition, 1.0);
    gl_Position = ubo.proj * ubo.view * vertexPos;

    normalVector = vec3(inModel * vec4(inNormalVector, 0.0));

    fragColor = inColor;
    fragTexCoord = inTexCoord;
    fragMaterial = inMaterial;
}

