#include "VulkanApp.h"
#include "VlkRenderer.h"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

VlkRenderer vlkRenderer;
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

void VulkanApp::initWindow() {
  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window = glfwCreateWindow(WIDTH, HEIGHT, "VulkanApp", nullptr, nullptr);
}

void VulkanApp::initVulkan() {
  vlkRenderer.createInstance();

  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions;

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &vlkRenderer.appInfo;
  createInfo.enabledExtensionCount = glfwExtensionCount;
  createInfo.ppEnabledExtensionNames = glfwExtensions;
  createInfo.enabledLayerCount = 0;

  if (vkCreateInstance(&createInfo, nullptr, &vlkRenderer.instance) !=
      VK_SUCCESS) {
    throw std::runtime_error("Failed to create instance!");
  }

  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

  std::vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                         extensions.data());

  std::cout << "Available Extensions:\n";

  for (const auto &extension : extensions) {
    std::cout << '\t' << extension.extensionName << '\n';
  }
}

void VulkanApp::mainLoop() {
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }
}

void VulkanApp::cleanup() {
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
