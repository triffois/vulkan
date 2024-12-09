#include "SceneLighting.h"
#include <cstring>

SceneLighting::SceneLighting(Device &deviceToUse, size_t attachmentBindingIndex)
    : lightingAttachment(
        &deviceToUse,
        [](SimpleLightSource newLights[3]) {
            const std::vector<SimpleLightSource> vec = {
                SimpleLightSource{
                    glm::vec4{0.0, 0.8, 0.7, 0.0},
                    glm::vec4{24.0, 14.0, 7.0, 0.0}, 0.75f, 25
                },
                SimpleLightSource{
                    glm::vec4{0.5, 0.15, 0.3, .0},
                    glm::vec4{35.0, 20.0, 10.0, 0.0}, 0.7f,
                    30
                },
                SimpleLightSource{
                    glm::vec4{0.35, 0.25, 0.9, 0.0},
                    glm::vec4{11.0, 35.0, 29.0, 0.0}, 0.85f,
                    10
                },
            };

            memcpy(newLights, vec.data(), 3 * sizeof(SimpleLightSource));
        },
        attachmentBindingIndex) {
}

UniformAttachment<SimpleLightSource[3]> &SceneLighting::getLightingBuffer() {
    return lightingAttachment;
}
