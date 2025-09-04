# TerraClone++

TerraClone++ is a 2D tile-based game inspired by Terraria, built using the Vulkan graphics API. The project aims to replicate the core rendering of a tile-based world with textured quads, leveraging Vulkan for high-performance 2D graphics.
Currently, the project renders a single textured quad in a 2D orthographic space, with plans to expand to a full tile grid for a Terraria-like world.

## Features:
- 2D rendering with Vulkan, using an orthographic projection
- Support for textured quads (16x16 pixels) using texture atlas.
- Uniform Buffer Objects for model-view-projection matrices.
- Basic Vulkan rendering pipeline based on the Vulkan Tutorial
- Window management with GLFW.
- Texture loading with stb_image

## Prerequisites:
- **VulkanSDK**: Install the latest VulkanSDK (e.g., LunarG VulkanSDK).
- **C++ Compiler**: A C++20 compatible compiler (e.g, GCC, Clang).
- **GLFW**: For window and input handling.
- **GLM**: For matrix and vector math.
- **stb_image**: For loading texture images.
- **CMake**: For building the project.
- **glslc**: For compiling GLSL to SPIR-V
