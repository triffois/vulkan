#version 450

// attachments input
layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    vec4 camPos;
}
ubo;

layout(binding = 1) uniform Resolutions { vec4 textureResolutions[256]; }
res;

layout(binding = 2) uniform sampler2DArray texSampler;

struct SimpleLight {
    vec4 lightColor;
    vec4 lightPos;
    float lightIntensity;
    int lightRange;
};

struct DirectionalLightSource {
    vec4 lightDirection;
    vec4 lightColor;
};

const int nSimpleLights = 3;
layout(set = 0, binding = 3) uniform SceneLightData {
    SimpleLight lights[nSimpleLights];
    DirectionalLightSource dirLight;
}
sceneLights;

// fragment shader input
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec4 vertexPos;
layout(location = 3) in vec3 normalVector;
layout(location = 4) flat in ivec4 material;

layout(location = 0) out vec4 outColor;

void main() {
    int textureIndex = material.x;

    vec4 textureColor;
    if (textureIndex < 0) {
        textureColor = vec4(fragColor, 1);
    } else {
        vec2 resolution = res.textureResolutions[textureIndex].xy;
        vec2 scaledUV = fragTexCoord * resolution;
        textureColor = texture(texSampler, vec3(scaledUV, float(textureIndex)));
    }

    outColor = vec4(vec3(0), 1);

    vec3 viewDirVec3 = vec3(normalize(ubo.camPos - vertexPos));

    vec3 vertNormal = normalize(normalVector);

    float distanceBase = 0.9;

    float ambientAmount = 0.35f;
    float specularAmount = 0.6f;

    for (int i = 0; i < nSimpleLights; ++i) {
        vec3 LightposVec3 = sceneLights.lights[i].lightPos.xyz;

        vec3 lightDir = LightposVec3 - vertexPos.xyz;

        float intensityFaint =
        pow(distanceBase,
        length(lightDir) / sceneLights.lights[i].lightRange) *
        sceneLights.lights[i].lightIntensity;

        vec3 lightDirNorm = normalize(lightDir);

        // compute diffuse fraction
        float diffuse =
        max(dot(vertNormal, lightDirNorm), 0.0) * intensityFaint;

        // compute specular fraction
        vec3 halfway = normalize(lightDirNorm + viewDirVec3);
        float specular = pow(max(dot(vertNormal, halfway), 0.0f), 16) *
        specularAmount * intensityFaint;

        outColor += (specular + diffuse) * vec4(sceneLights.lights[i].lightColor.xyz, 0.0);
    }

    //directional light
    vec4 directionalDiffuse =
    (max(dot(vertNormal, normalize(sceneLights.dirLight.lightDirection).xyz), 0.0) + ambientAmount) * vec4(sceneLights.dirLight.lightColor.xyz, 0.0);
    outColor += directionalDiffuse;

    outColor *= textureColor;
    outColor.w = textureColor.w;
}

