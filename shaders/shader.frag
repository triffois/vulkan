#version 450

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
    int lightRange;
};

const int nSimpleLights = 3;
layout(set = 0, binding = 2) uniform lights{ SimpleLight lights[nSimpleLights]; } simpleLights;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec4 vertexPos;
layout(location = 3) in vec3 normalVector;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 viewDirVec3 = vec3(normalize(ubo.camPos - vertexPos));

    vec3 vertNormal = normalize(normalVector);
    vec4 textureColor = texture(texSampler, fragTexCoord);

    float distanceBase = 0.9;

    float ambientAmount = 0.35f;
    float specularAmount = 0.45f;

    for(int i = 0; i < nSimpleLights; ++i)
    {
        vec3 LightposVec3 = vec3(simpleLights.lights[i].lightPos.x, simpleLights.lights[i].lightPos.y, simpleLights.lights[i].lightPos.z);

        vec3 lightDir = LightposVec3 - vec3(vertexPos.x, vertexPos.y, vertexPos.z);

        float intensityFaint = pow(distanceBase, length(lightDir) / simpleLights.lights[i].lightRange) * simpleLights.lights[i].lightIntensity;

        vec3 lightDirNorm = normalize(lightDir);

        //compute diffuse fraction
        float diffuse = max(dot(vertNormal, lightDirNorm), 0.0) * intensityFaint + ambientAmount;

        //compute specular fraction
        vec3 halfway = normalize(lightDirNorm + viewDirVec3);
        float specular = pow(max(dot(vertNormal, halfway), 0.0f), 16) * specularAmount * intensityFaint;

        outColor += textureColor * simpleLights.lights[i].lightColor * (specular + diffuse);
    }
}