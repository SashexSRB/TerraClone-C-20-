#pragma once

#include <vector>
#include <vulkan/vulkan.h>

class VlkRenderer {
public:
  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions;

  VkInstance instance;

  void createInstance();
  std::vector<const char *> getRequiredExtensions();
};
