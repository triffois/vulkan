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

const int nSimpleLights = 3;
layout(set = 0, binding = 3) uniform lights {
    SimpleLight lights[nSimpleLights];
}
simpleLights;

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
        textureColor = vec4(fragColor, 1.0);
    } else {
        vec2 resolution = res.textureResolutions[textureIndex].xy;
        vec2 scaledUV = fragTexCoord * resolution;
        textureColor = texture(texSampler, vec3(scaledUV, float(textureIndex)));
    }

    vec3 viewDirVec3 = vec3(normalize(ubo.camPos - vertexPos));

    vec3 vertNormal = normalize(normalVector);

    float distanceBase = 0.9;

    float ambientAmount = 0.35f;
    float specularAmount = 0.45f;

    outColor = vec4(0, 0, 0, 1);

    for (int i = 0; i < nSimpleLights; ++i) {
        vec3 LightposVec3 = simpleLights.lights[i].lightPos.xyz;

        vec3 lightDir = LightposVec3 - vertexPos.xyz;

        float intensityFaint =
            pow(distanceBase,
                length(lightDir) / simpleLights.lights[i].lightRange) *
            simpleLights.lights[i].lightIntensity;

        vec3 lightDirNorm = normalize(lightDir);

        // compute diffuse fraction
        float diffuse =
            max(dot(vertNormal, lightDirNorm), 0.0) * intensityFaint +
            ambientAmount;

        // compute specular fraction
        vec3 halfway = normalize(lightDirNorm + viewDirVec3);
        float specular = pow(max(dot(vertNormal, halfway), 0.0f), 16) *
                         specularAmount * intensityFaint;

        outColor += textureColor * simpleLights.lights[i].lightColor *
                    (specular + diffuse);
    }
}

