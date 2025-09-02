#include "VulkanApp.h"
#include "VlkRenderer.h"
#include "VlkValidator.h"
#include <iostream>

VlkRenderer vlkRenderer;

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

void VulkanApp::initWindow() {
  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window = glfwCreateWindow(WIDTH, HEIGHT, "VulkanApp", nullptr, nullptr);
  std::cout << "OK: Window Initialized.\n";
}

void VulkanApp::initVulkan() {
  vlkRenderer.createInstance();
  validator.setupDebugMessenger(vlkRenderer.instance);
  vlkRenderer.pickPhysicalDevice();

  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

  std::vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                         extensions.data());

  std::cout << "Available Extensions:\n";

  for (const auto &extension : extensions)
    std::cout << '\t' << extension.extensionName << '\n';
}

void VulkanApp::mainLoop() {
  while (!glfwWindowShouldClose(window))
    glfwPollEvents();
}

void VulkanApp::cleanup() {
  if (validator.enableValidationLayers)
    validator.DestroyDebugUtilsMessengerEXT(vlkRenderer.instance,
                                            validator.debugMessenger, nullptr);

  vkDestroyInstance(vlkRenderer.instance, nullptr);

  glfwDestroyWindow(window);

  glfwTerminate();
}

void VulkanApp::run() {
  VulkanApp::initWindow();
  VulkanApp::initVulkan();
  VulkanApp::mainLoop();
  VulkanApp::cleanup();
}
