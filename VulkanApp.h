#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

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
