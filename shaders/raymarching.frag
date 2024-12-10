#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    vec4 camPos;
}
ubo;

// Light data from your original setup
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
layout(set = 0, binding = 1) uniform SceneLightData {
    SimpleLight lights[nSimpleLights];
    DirectionalLightSource dirLight;
}
sceneLights;

layout(location = 0) in vec3 rayOrigin;
layout(location = 1) in vec3 nearPos;

layout(location = 0) out vec4 outColor;

// Your existing helper functions
float smoothmin(float a, float b, float k) {
    float h = clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
    return mix(b, a, h) - k * h * (1.0 - h);
}

float sdfSphere(vec3 p, vec3 center, float radius) {
    return length(p - center) - radius;
}

float scene(vec3 p) {
    float d = 1e10;

    // Define spheres directly in world space
    for (int i = 0; i < 5; i++) {
        vec3 center = vec3(sin(float(i) * 1.2), cos(float(i) * 0.8) * 0.5,
                           sin(float(i) * 0.9) * 0.5) +
                      vec3(2, 1.5, 0);
        float radius = 0.3;
        d = smoothmin(d, sdfSphere(p, center, radius), 0.2);
    }
    return d;
}

vec3 calculateNormal(vec3 p) {
    float eps = 0.01;
    vec3 dx = vec3(eps, 0.0, 0.0);
    vec3 dy = vec3(0.0, eps, 0.0);
    vec3 dz = vec3(0.0, 0.0, eps);

    vec3 normal =
        vec3(scene(p + dx) - scene(p - dx), scene(p + dy) - scene(p - dy),
             scene(p + dz) - scene(p - dz));

    return normalize(normal);
}

float shadow(vec3 p, vec3 lightDir) {
    float t = 0.1;
    for (int i = 0; i < 50; i++) {
        float d = scene(p + t * lightDir);
        if (d < 0.001)
            return 0.0;
        t += d;
        if (t > 10.0)
            break;
    }
    return 1.0;
}

vec3 computeLighting(vec3 p, vec3 normal) {
    vec3 color = vec3(0.0);

    // Point lights
    for (int i = 0; i < nSimpleLights; i++) {
        vec3 lightDir = normalize(sceneLights.lights[i].lightPos.xyz - p);
        float diff = max(dot(normal, lightDir), 0.0);
        float shadowFactor = shadow(p + normal * 0.01, lightDir);
        float distFactor =
            pow(0.9, length(sceneLights.lights[i].lightPos.xyz - p) /
                         float(sceneLights.lights[i].lightRange));
        color += sceneLights.lights[i].lightColor.rgb * diff * shadowFactor *
                 sceneLights.lights[i].lightIntensity * distFactor;
    }

    // Directional light
    vec3 dirLightDir = normalize(sceneLights.dirLight.lightDirection.xyz);
    float dirDiff = max(dot(normal, -dirLightDir), 0.0);
    float dirShadowFactor = shadow(p + normal * 0.01, -dirLightDir);
    color += sceneLights.dirLight.lightColor.rgb * dirDiff * dirShadowFactor;

    return color;
}

void main() {
    // Normalize the ray direction
    vec3 rd = normalize(nearPos - rayOrigin);
    float t = 0.0;
    vec3 col = vec3(0.0);

    // Start raymarching from the camera position
    vec3 p = rayOrigin;

    // Adjust max distance to be larger since we're starting from the camera
    float maxDist = 50.0;
    float totalDistance = 0.0;

    // Raymarching loop
    for (int i = 0; i < 100; i++) {
        float dist = scene(p);

        if (dist < 0.001) {
            vec3 normal = calculateNormal(p);
            col = computeLighting(p, normal);
            break;
        }

        t += dist;
        p = rayOrigin + rd * t;

        totalDistance += dist;
        if (totalDistance > maxDist)
            break;
    }

    // Background color if no hit
    if (totalDistance > maxDist) {
        outColor = vec4(0.0);
        return;
    }

    outColor = vec4(col, 1.0);
}
