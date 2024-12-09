#include "SceneLighting.h"
#include <cstring>

SceneLighting::SceneLighting(Device &deviceToUse, size_t attachmentBindingIndex)
    : lightingAttachment(
          &deviceToUse,
          [](SimpleLightSource newLights[3]) {
              const std::vector<SimpleLightSource> vec = {
                  SimpleLightSource{glm::vec4{24.0, 27.0, 35.0, 1.0},
                                    glm::vec4{24.0, 14.0, 7.0, 0.0}, 0.5f, 200},
                  SimpleLightSource{glm::vec4{52.0, 39.0, 29.0, 1.0},
                                    glm::vec4{35.0, 20.0, 10.0, 0.0}, 0.7f,
                                    150},
                  SimpleLightSource{glm::vec4{9.0, 35.0, 18.0, 1.0},
                                    glm::vec4{11.0, 35.0, 29.0, 0.0}, 0.65f,
                                    250},
              };

              memcpy(newLights, vec.data(), 3 * sizeof(SimpleLightSource));
          },
          attachmentBindingIndex) {}

UniformAttachment<SimpleLightSource[3]> &SceneLighting::getLightingBuffer() {
    return lightingAttachment;
}
