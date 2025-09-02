#pragma once

#include <optional>
#include <vector>
#include <vulkan/vulkan.h>

class VlkRenderer {
public:
  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions;

  struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;

    bool isComplete() { return graphicsFamily.has_value(); }
  };

  VkInstance instance;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkPhysicalDeviceProperties deviceProperties;
  VkPhysicalDeviceFeatures deviceFeatures{};
  VkDevice device;
  VkQueue graphicsQueue;

  void createInstance();
  void pickPhysicalDevice();
  void createLogicalDevice();
  int rateDeviceSuitability(VkPhysicalDevice device);
  bool isDeviceSuitable(VkPhysicalDevice device);

  std::vector<const char *> getRequiredExtensions();

  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
};
