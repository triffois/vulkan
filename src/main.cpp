#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <random>

#include "Engine.h"
#include "Model.h"
#include "ModelLoader.h"
#include "commonstructs.h"

std::vector<PerInstanceData> generateRandomCube(size_t sideLength, size_t gapSize) {
    std::vector<PerInstanceData> instanceTransforms{sideLength * sideLength * sideLength, PerInstanceData{glm::mat4{1.0f}}};

    std::default_random_engine              rnd_generator(static_cast<unsigned>(time(nullptr)));
    std::uniform_real_distribution<float>   uniform_dist(0.5f, 2.0f);

    auto timeStart = std::chrono::high_resolution_clock::now();

    size_t modelIdx = 0;
    for (size_t x = 0; x < sideLength; ++x) {
        for (size_t y = 0; y < sideLength; ++y) {
            for (size_t z = 0; z < sideLength; ++z, ++modelIdx) {
                //scaling transofrm
                auto randomScaleFactor = uniform_dist(rnd_generator);
                instanceTransforms[modelIdx].instanceWorldTransform = glm::scale(instanceTransforms[modelIdx].instanceWorldTransform, glm::vec3{randomScaleFactor, randomScaleFactor, randomScaleFactor});

                //shifting transform
                glm::vec3 position{x*gapSize, y*gapSize, z*gapSize};
                instanceTransforms[modelIdx].instanceWorldTransform = glm::translate(instanceTransforms[modelIdx].instanceWorldTransform, position);

                //rotating transform
                auto time_diff = std::chrono::high_resolution_clock::now() - timeStart;
                instanceTransforms[modelIdx].instanceWorldTransform = glm::rotate(instanceTransforms[modelIdx].instanceWorldTransform, std::chrono::duration<float, std::chrono::microseconds::period>(time_diff).count() * glm::radians(90.0f), glm::normalize(glm::vec3{1.0f * randomScaleFactor,0.0f, 1.0f}));
            }
        }
    }

    return instanceTransforms;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_gltf_file> "
                  << std::endl;
        return EXIT_FAILURE;
    }

    Engine engine;
    std::vector<Model> models = ModelLoader::loadFromGLTF(argv[1]);

    // Create pipeline(s) outside the engine
    std::vector<Pipeline> pipelines;

    bool useInstancedRendering = true;
    size_t numInstances {5};

    if (useInstancedRendering) {

        auto randomPoses = generateRandomCube(numInstances, 7);

        for (const auto &model : models) {
            pipelines.push_back(engine.createPipelineInstanced("shaders/vert_instanced.spv",
                                                      "shaders/frag.spv", model, randomPoses));
        }
    }else {
        for (const auto &model : models) {
            pipelines.push_back(engine.createPipeline("shaders/vert.spv",
                                                      "shaders/frag.spv", model));
        }
    }

    try {
        while (engine.running()) {
            auto render = engine.startRender();

            for (const auto &pipeline : pipelines) {
                render.submit(pipeline);
            }

            engine.finishRender(render);
        }

    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
