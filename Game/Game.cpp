// Game/Game.cpp
#include "Game.h"
#include "../VulkanApp.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

VulkanApp app;

Game::Game(GLFWwindow *window, VlkRenderer &renderer)
    : renderer(renderer), window(window), world(100, 50), player(),
      worldChanged(true) {
  player.position = {50.0 * 16.0, 24.0 * 16.0};
  world.generate();
  updateBuffers();
}

void Game::notifyWorldChanged() { worldChanged = true; }

void Game::run() {
  static float lastTime = glfwGetTime();
  float currentTime = glfwGetTime();
  float deltaTime = currentTime - lastTime;
  lastTime = currentTime;
  update(deltaTime);
  handleInput();
  updateBuffers();
  renderer.drawFrame(window, app.framebufferResized);
}

void Game::update(float deltaTime) {
  // Player movement
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    player.velocity.x = -player.moveSpeed;
  } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    player.velocity.x = player.moveSpeed;
  } else {
    player.velocity.x = 0.0f;
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && player.isGrounded) {
    player.velocity.y = -player.jumpSpeed;
    player.isGrounded = false;
  }
  player.velocity.y += 800.0f * deltaTime; // Gravity
  player.position += player.velocity * deltaTime;

  // Basic collision (ground at y=25 * 16)
  float groundY = world.getHeight() / 2 * 16.0f;
  if (player.position.y > groundY) {
    player.position.y = groundY;
    player.velocity.y = 0.0f;
    player.isGrounded = true;
  }
}

void Game::handleInput() {
  float moveSpeed = player.moveSpeed;
  player.velocity.x = 0.0f;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    player.velocity.x = -moveSpeed;
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    player.velocity.x = moveSpeed;
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && player.isGrounded) {
    player.velocity.y = -player.jumpSpeed;
    player.isGrounded = false;
  }
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    int tileX = static_cast<int>(
        (xpos + player.position.x - renderer.swapChainExtent.width / 2.0f) /
        16.0f);
    int tileY = static_cast<int>(
        (ypos + player.position.y - renderer.swapChainExtent.height / 2.0f) /
        16.0f);
    if (tileX >= 0 && tileX < world.getWidth() && tileY >= 0 &&
        tileY < world.getHeight()) {
      Tile &tile = world.getTile(tileX, tileY);
      if (tile.isActive && TileRegistry::tileTypes[tile.tileId].isSolid) {
        player.inventory.addItem(tile.tileId, 1);
        tile.isActive = false;
        notifyWorldChanged();
      }
    }
  }
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    int tileX = static_cast<int>(
        (xpos + player.position.x - renderer.swapChainExtent.width / 2.0f) /
        16.0f);
    int tileY = static_cast<int>(
        (ypos + player.position.y - renderer.swapChainExtent.height / 2.0f) /
        16.0f);
    if (tileX >= 0 && tileX < world.getWidth() && tileY >= 0 &&
        tileY < world.getHeight()) {
      Tile &tile = world.getTile(tileX, tileY);
      if (!tile.isActive && !player.inventory.items.empty()) {
        tile.tileId = player.inventory.items[0].first;
        tile.isActive = true;
        player.inventory.items[0].second--;
        if (player.inventory.items[0].second <= 0) {
          player.inventory.items.erase(player.inventory.items.begin());
        }
        notifyWorldChanged();
      }
    }
  }
}

void Game::updateBuffers() {
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
  if (worldChanged) {
    world.generateVertices(worldVertices, worldIndices);
    worldChanged = false;
    std::cout << "World generated: " << worldVertices.size() << " vertices, "
              << worldIndices.size() << " indices\n";
  }
  vertices = worldVertices;
  indices = worldIndices;
  std::vector<Vertex> playerVertices;
  std::vector<uint32_t> playerIndices;
  generatePlayerVertices(player, playerVertices, playerIndices);
  std::vector<Vertex> uiVertices;
  std::vector<uint32_t> uiIndices;
  generateInventoryVertices(player.inventory, renderer.swapChainExtent.width,
                            renderer.swapChainExtent.height, uiVertices,
                            uiIndices);
  uint32_t baseIndex = static_cast<uint32_t>(vertices.size());
  vertices.insert(vertices.end(), playerVertices.begin(), playerVertices.end());
  for (uint32_t idx : playerIndices)
    indices.push_back(baseIndex + idx);
  baseIndex = static_cast<uint32_t>(vertices.size());
  vertices.insert(vertices.end(), uiVertices.begin(), uiVertices.end());
  for (uint32_t idx : uiIndices)
    indices.push_back(baseIndex + idx);
  std::cout << "Total: " << vertices.size() << " vertices, " << indices.size()
            << " indices\n";

  // TODO:
  // renderer.updateVertexBuffer(vertices);
  // renderer.updateIndexBuffer(indices);
  renderer.updateUniformBuffer(0);
}
