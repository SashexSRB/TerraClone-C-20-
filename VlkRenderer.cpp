#include "VlkRenderer.h"
#include "VlkValidator.h"
#include <GLFW/glfw3.h>
// #include <map>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

std::vector<const char *> VlkRenderer::getRequiredExtensions() {
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  std::vector<const char *> extensions(glfwExtensions,
                                       glfwExtensions + glfwExtensionCount);

  if (validator.enableValidationLayers)
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  return extensions;
};

void VlkRenderer::createInstance() {
  if (validator.enableValidationLayers &&
      !validator.checkValidationLayerSupport())
    throw std::runtime_error("Validation layers requested, but not available");

  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "VulkanApp";
  appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
  appInfo.pEngineName = "VlkEngine";
  appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  auto extensions = getRequiredExtensions();
  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  createInfo.ppEnabledExtensionNames = extensions.data();

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
  if (validator.enableValidationLayers) {
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(validator.validationLayers.size());
    createInfo.ppEnabledLayerNames = validator.validationLayers.data();

    validator.populateDebugMesengerCreateInfo(debugCreateInfo);
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
  } else {
    createInfo.enabledLayerCount = 0;
    createInfo.pNext = nullptr;
  }
  if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
    throw std::runtime_error("Failed to create instance!");

  std::cout << "OK: Instance created.\n";
}

VlkRenderer::QueueFamilyIndices
VlkRenderer::findQueueFamilies(VkPhysicalDevice device) {
  QueueFamilyIndices indices;
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                           queueFamilies.data());

  int i = 0;
  for (const auto &queueFamily : queueFamilies) {
    if (indices.isComplete())
      break;

    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
      indices.graphicsFamily = i;

    i++;
  }
  std::cout << "OK: Queue Families: " << i << "\n";

  return indices;
}

void VlkRenderer::createLogicalDevice() {
  QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

  VkDeviceQueueCreateInfo queueCreateInfo{};
  queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
  queueCreateInfo.queueCount = 1;

  float queuePriority = 1.0f;
  queueCreateInfo.pQueuePriorities = &queuePriority;

  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.pQueueCreateInfos = &queueCreateInfo;
  createInfo.queueCreateInfoCount = 1;

  createInfo.pEnabledFeatures = &deviceFeatures;
  createInfo.enabledExtensionCount = 0;

  if (validator.enableValidationLayers) {
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(validator.validationLayers.size());
    createInfo.ppEnabledLayerNames = validator.validationLayers.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }

  if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) !=
      VK_SUCCESS)
    throw std::runtime_error("Failed to create logical device!");

  vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);

  std::cout << "OK: Logical device created.\n";
}

bool VlkRenderer::isDeviceSuitable(VkPhysicalDevice device) {
  // vkGetPhysicalDeviceProperties(device, &deviceProperties);
  // VK_EXT_DEBUG_UTILS_EXTENSION_NAMEhysicalDeviceFeatures(device,
  // &deviceFeatures); return deviceProperties.deviceType ==
  // VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
  //        deviceFeatures.geometryShader;

  QueueFamilyIndices indices = findQueueFamilies(device);

  return indices.isComplete();
}

int VlkRenderer::rateDeviceSuitability(VkPhysicalDevice device) {
  int score = 0;

  if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    score += 1000;

  score += deviceProperties.limits.maxImageDimension2D;

  if (!deviceFeatures.geometryShader) {
    return 0;
  }

  return score;
}

void VlkRenderer::pickPhysicalDevice() {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

  if (deviceCount == 0)
    throw std::runtime_error("Failed to find GPUs with Vulkan support!");

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

  // std::multimap<int, VkPhysicalDevice> candidates;

  /*for (const auto &device : devices) {
    int score = rateDeviceSuitability(device);
    candidates.insert(std::make_pair(score, device));
  }

  if (candidates.rbegin()->first > 0) {
    physicalDevice = candidates.rbegin()->second;
  } else {
    throw std::runtime_error("Failed to find a suitable GPU");
  }*/

  for (const auto &device : devices) {
    if (isDeviceSuitable(device)) {
      physicalDevice = device;
      break;
    }
  }

  if (physicalDevice == VK_NULL_HANDLE)
    throw std::runtime_error("Failed to find a suitable GPU");
  std::cout << "OK: Physical Device Selected.\n";
}
