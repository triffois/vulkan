# Vulkan-based Rendering Engine

A lightweight rendering engine built on top of Vulkan, designed to abstract away the low-level complexity while maintaining flexibility. The engine provides a simple API for loading and rendering 3D scenes while allowing direct access to the underlying graphics pipeline for experimentation.

## Building and Running

### Prerequisites

- Vulkan SDK
- GLFW3
- GLM
- CMake (3.22+)
- A C++17 compliant compiler

### Building

```shell
mkdir build && cd build
cmake ..
make
```

### Shader Compilation

The engine expects compiled SPIR-V shaders in the `./shaders/` directory relative to where you run the executable. To compile the shaders, use:

```shell
cd shaders
glslc shader.vert -o vert.spv
glslc shader.frag -o frag.spv
```

Or use the provided shell script (requires path to glslc):

```shell
cd shaders
./compile.sh /path/to/glslc
```

### Running the demo

```shell
./build/RenderEngine path/to/your/model.gltf
```

Note: the demo looks for `shaders/frag.spv` and `shaders/vert.spv` in the current working directory, instead of looking for them more intelligently.

## Controls

The demo uses a first-person camera control scheme:

- W/S - Move forward/backward
- A/D - Strafe left/right
- Space/Shift - Move up/down
- Mouse - Look around
- Scroll - Zoom in/out

## Extending the Engine

The engine is designed to be extensible. Here are some quick ways to experiment:

1. **Custom Shading**: The fastest way to experiment is by modifying the fragment shader (`shader.frag`). You can implement different lighting models or add post-processing effects.

2. **New Renderables**: The engine's renderable system is extensible - you can create new types of renderables by implementing custom render passes.

3. **Custom Attachments**: Add new uniform buffers or texture attachments to pass additional data to your shaders.

For example, to modify the lighting model, you would edit the fragment shader's main function in `shaders/shader.frag`:

```glsl
void main() {
    // Your custom lighting calculations here
}
```

Remember to recompile shaders after any modifications.
