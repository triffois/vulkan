#version 450

const int nSimpleLights = 1;

layout(binding = 1) uniform sampler2D texSampler;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec4 camPos;
} ubo;

struct SimpleLight
{
    vec4 lightColor;
    vec4 lightPos;
    float lightIntensity;
};

layout(set = 0, binding = 2) uniform StaticLights{ SimpleLight simpleLights[nSimpleLights]; } staticLights;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec4 vertexPos;
layout(location = 3) in vec3 normalVector;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 viewDir = vec3(normalize(ubo.camPos - vertexPos).x, normalize(ubo.camPos - vertexPos).y, normalize(ubo.camPos - vertexPos).z);
    vec3 vertNormal = normalize(normalVector);
    vec4 textureColor = texture(texSampler, fragTexCoord);

    for(int i = 0; i < nSimpleLights; ++i)
    {
        float ambientAmount = 0.3f;
        vec3 lightDir = normalize(vec3(staticLights.simpleLights[i].lightPos.x, staticLights.simpleLights[i].lightPos.y, staticLights.simpleLights[i].lightPos.z) - vec3(vertexPos.x, vertexPos.y, vertexPos.z));
        float diffuse = max(dot(vertNormal, lightDir), 0.0) + ambientAmount;

        float specularAmount = 0.45f;
        vec3 halfway = normalize(lightDir + viewDir);
        float specular = pow(max(dot(halfway, vertNormal), 0.0f), 8) * specularAmount;

        outColor += textureColor * (specular + diffuse) * staticLights.simpleLights[i].lightColor;
    }
}