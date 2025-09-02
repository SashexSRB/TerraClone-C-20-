#include "VlkRenderer.h"
#include "VlkValidator.h"
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

std::vector<const char *> VlkRenderer::getRequiredExtensions() {
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  std::vector<const char *> extensions(glfwExtensions,
                                       glfwExtensions + glfwExtensionCount);

  if (validator.enableValidationLayers) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  return extensions;
};

void VlkRenderer::createInstance() {
  if (validator.enableValidationLayers &&
      !validator.checkValidationLayerSupport()) {
    throw std::runtime_error("Validation layers requested, but not available");
  }

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
  if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create instance!");
  }
}
