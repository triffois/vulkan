#include "SceneLighting.h"

#include <chrono>
#include <cstring>

SceneLighting::SceneLighting(Device &deviceToUse, size_t attachmentBindingIndex)
    : lightingAttachment(
          &deviceToUse,
          [](LightsArray &array) {
              int radius = 7;
              const auto time = ((std::chrono::high_resolution_clock::now()
                                      .time_since_epoch() /
                                  std::chrono::milliseconds(1))) /
                                1000.0;

              auto coord1 = radius * std::sin(time * 3.14);
              auto coord2 = radius * std::cos(time * 3.14);

              const std::vector<SimpleLightSource> vec = {
                  SimpleLightSource{glm::vec4{0.2, 0.8, 0.7, 0.0},
                                    glm::vec4{coord1, 3.0, coord2, 0.0}, 1.0f,
                                    5},
                  SimpleLightSource{glm::vec4{0.5, 0.15, 0.3, .0},
                                    glm::vec4{2.0, coord1, coord2, 0.0}, 1.0f,
                                    5},
                  SimpleLightSource{glm::vec4{0.35, 0.25, 0.9, 0.0},
                                    glm::vec4{coord1, coord2, 4.0f, 0.0}, 1.0f,
                                    5},
              };

              memcpy(array.lights, vec.data(), 3 * sizeof(SimpleLightSource));
          },
          attachmentBindingIndex) {}

UniformAttachment<LightsArray> &SceneLighting::getLightingBuffer() {
    return lightingAttachment;
}
