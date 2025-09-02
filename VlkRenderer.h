#pragma once

#include <vulkan/vulkan.h>

class VlkRenderer {
public:
  VkInstance instance;
  VkApplicationInfo appInfo{};

  void createInstance();
};
