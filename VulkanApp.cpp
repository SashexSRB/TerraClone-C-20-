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
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

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
  vlkRenderer.createGraphicsPipeline();
  vlkRenderer.createFramebuffers();
  vlkRenderer.createCommandPool();
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

void VulkanApp::drawFrame() {
  vkWaitForFences(vlkRenderer.device, 1,
                  &vlkRenderer.inFlightFences[currentFrame], VK_TRUE,
                  UINT64_MAX);

  uint32_t imageIndex;
  VkResult result = vkAcquireNextImageKHR(
      vlkRenderer.device, vlkRenderer.swapChain, UINT64_MAX,
      vlkRenderer.imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE,
      &imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    vlkRenderer.recreateSwapChain(window);
    return;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("Failed to acquire swap chain image!");
  }

  vkResetFences(vlkRenderer.device, 1,
                &vlkRenderer.inFlightFences[currentFrame]);

  vkResetCommandBuffer(vlkRenderer.commandBuffers[currentFrame], 0);
  vlkRenderer.recordCommandBuffer(vlkRenderer.commandBuffers[currentFrame],
                                  imageIndex);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {
      vlkRenderer.imageAvailableSemaphores[currentFrame]};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &vlkRenderer.commandBuffers[currentFrame];

  VkSemaphore signalSemaphores[] = {
      vlkRenderer.renderFinishedSemaphores[currentFrame]};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  if (vkQueueSubmit(vlkRenderer.graphicsQueue, 1, &submitInfo,
                    vlkRenderer.inFlightFences[currentFrame]) != VK_SUCCESS)
    throw std::runtime_error("Failed to submit draw command buffer!");

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = {vlkRenderer.swapChain};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &imageIndex;

  presentInfo.pResults = nullptr;

  result = vkQueuePresentKHR(vlkRenderer.presentQueue, &presentInfo);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      framebufferResized) {
    framebufferResized = false;
    vlkRenderer.recreateSwapChain(window);
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("Failed to present swap chain image!");
  }

  currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VulkanApp::mainLoop() {
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    drawFrame();
  }

  vkDeviceWaitIdle(vlkRenderer.device);
}

void VulkanApp::cleanup() {
  vlkRenderer.cleanupSwapChain();

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
