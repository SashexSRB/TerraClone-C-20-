#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class VulkanApp {
public:
  bool framebufferResized = false;

  void run();

private:
  GLFWwindow *window;
  uint32_t currentFrame = 0;

  void initVulkan();
  void mainLoop();
  void cleanup();
  void initWindow();

  void drawFrame();
};
