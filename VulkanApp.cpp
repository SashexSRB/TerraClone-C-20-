#include "VulkanApp.h"
#include "VlkRenderer.h"
#include "VlkValidator.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <vulkan/vulkan_core.h>

VlkRenderer vlkRenderer;

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;
const int MAX_FRAMES_IN_FLIGHT = 2;

void VulkanApp::initWindow() {
  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  window = glfwCreateWindow(WIDTH, HEIGHT, "VulkanApp", nullptr, nullptr);
  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, vlkRenderer.framebufferResizeCallback);
  std::cout << "OK: Window Initialized.\n";
}

void VulkanApp::initVulkan() {
  vlkRenderer.createInstance();
  validator.setupDebugMessenger(vlkRenderer.instance);
  vlkRenderer.createSurface(window);
  vlkRenderer.pickPhysicalDevice();
  vlkRenderer.createLogicalDevice();
  vlkRenderer.createSwapChain(window);
  vlkRenderer.createImageViews();
  vlkRenderer.createRenderPass();
  vlkRenderer.createDescriptorSetLayout();
  vlkRenderer.createGraphicsPipeline();
  vlkRenderer.createCommandPool();
  vlkRenderer.createDepthResources();
  vlkRenderer.createFramebuffers();
  vlkRenderer.createTextureImage();
  vlkRenderer.createTextureImageView();
  vlkRenderer.createSampler();
  vlkRenderer.createVertexBuffer();
  vlkRenderer.createIndexBuffer();
  vlkRenderer.createUniformBuffers();
  vlkRenderer.createDescriptorPool();
  vlkRenderer.createDescriptorSets();
  vlkRenderer.createCommandBuffers();
  vlkRenderer.createSyncObjects();

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
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    vlkRenderer.drawFrame(window, framebufferResized);
  }

  vkDeviceWaitIdle(vlkRenderer.device);
}

void VulkanApp::cleanup() {
  vlkRenderer.cleanupSwapChain();

  vkDestroySampler(vlkRenderer.device, vlkRenderer.textureSampler, nullptr);

  vkDestroyImageView(vlkRenderer.device, vlkRenderer.textureImageView, nullptr);

  vkDestroyImage(vlkRenderer.device, vlkRenderer.textureImage, nullptr);
  vkFreeMemory(vlkRenderer.device, vlkRenderer.textureImageMemory, nullptr);

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroyBuffer(vlkRenderer.device, vlkRenderer.uniformBuffers[i], nullptr);
    vkFreeMemory(vlkRenderer.device, vlkRenderer.uniformBuffersMemory[i],
                 nullptr);
  }

  vkDestroyDescriptorPool(vlkRenderer.device, vlkRenderer.descriptorPool,
                          nullptr);

  vkDestroyDescriptorSetLayout(vlkRenderer.device,
                               vlkRenderer.descriptorSetLayout, nullptr);

  vkDestroyBuffer(vlkRenderer.device, vlkRenderer.indexBuffer, nullptr);
  vkFreeMemory(vlkRenderer.device, vlkRenderer.indexBufferMemory, nullptr);

  vkDestroyBuffer(vlkRenderer.device, vlkRenderer.vertexBuffer, nullptr);
  vkFreeMemory(vlkRenderer.device, vlkRenderer.vertexBufferMemory, nullptr);

  vkDestroyPipeline(vlkRenderer.device, vlkRenderer.graphicsPipeline, nullptr);
  vkDestroyPipelineLayout(vlkRenderer.device, vlkRenderer.pipelineLayout,
                          nullptr);

  vkDestroyRenderPass(vlkRenderer.device, vlkRenderer.renderPass, nullptr);

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(vlkRenderer.device,
                       vlkRenderer.imageAvailableSemaphores[i], nullptr);
    vkDestroySemaphore(vlkRenderer.device,
                       vlkRenderer.renderFinishedSemaphores[i], nullptr);
    vkDestroyFence(vlkRenderer.device, vlkRenderer.inFlightFences[i], nullptr);
  }

  vkDestroyCommandPool(vlkRenderer.device, vlkRenderer.commandPool, nullptr);

  vkDestroyDevice(vlkRenderer.device, nullptr);

  if (validator.enableValidationLayers)
    validator.DestroyDebugUtilsMessengerEXT(vlkRenderer.instance,
                                            validator.debugMessenger, nullptr);

  vkDestroySurfaceKHR(vlkRenderer.instance, vlkRenderer.surface, nullptr);

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
