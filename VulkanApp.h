#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class VulkanApp {
public:
  void run();

private:
  GLFWwindow *window;

  void initVulkan();
  void mainLoop();
  void cleanup();
  void initWindow();
};
