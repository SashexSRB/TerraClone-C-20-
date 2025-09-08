// Game/Game.cpp
#include "Game.h"
#include <glm/gtc/matrix_transform.hpp>

Game::Game(VlkRenderer &renderer, GLFWwindow *window)
    : renderer(renderer), window(window), world(100, 50) {
  TileRegistry::initialize();
  world.generate();
  player.position = {50 * 16.0f, world.getHeight() / 2 * 16.0f -
                                     32.0f}; // Start above ground
}

void Game::run() {
  static float lastTime = glfwGetTime();
  float currentTime = glfwGetTime();
  float deltaTime = currentTime - lastTime;
  lastTime = currentTime;
  update(deltaTime);
  handleInput();
  updateBuffers();
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
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    int tileX = static_cast<int>(mouseX / 16.0f);
    int tileY = static_cast<int>(mouseY / 16.0f);
    if (tileX >= 0 && tileX < world.getWidth() && tileY >= 0 &&
        tileY < world.getHeight()) {
      Tile &tile = world.getTile(tileX, tileY);
      if (tile.isActive) {
        player.inventory.addItem(tile.tileId, 1);
        tile.isActive = false;
        tile.tileId = 0;
      }
    }
  }
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    int tileX = static_cast<int>(mouseX / 16.0f);
    int tileY = static_cast<int>(mouseY / 16.0f);
    if (tileX >= 0 && tileX < world.getWidth() && tileY >= 0 &&
        tileY < world.getHeight()) {
      Tile &tile = world.getTile(tileX, tileY);
      if (!tile.isActive && !player.inventory.items.empty()) {
        tile.isActive = true;
        tile.tileId = player.inventory.items[0].first;
        player.inventory.items[0].second--;
        if (player.inventory.items[0].second <= 0) {
          player.inventory.items.erase(player.inventory.items.begin());
        }
      }
    }
  }
}

void Game::updateBuffers() {
  std::vector<Vertex> vertices;
  std::vector<uint16_t> indices;
  world.generateVertices(vertices, indices);
  std::vector<Vertex> playerVertices;
  std::vector<uint16_t> playerIndices;
  generatePlayerVertices(player, playerVertices, playerIndices);
  std::vector<Vertex> uiVertices;
  std::vector<uint16_t> uiIndices;
  generateInventoryVertices(player.inventory, renderer.swapChainExtent.width,
                            renderer.swapChainExtent.height, uiVertices,
                            uiIndices);

  uint32_t baseIndex = static_cast<uint32_t>(vertices.size());
  vertices.insert(vertices.end(), playerVertices.begin(), playerVertices.end());
  for (uint16_t idx : playerIndices)
    indices.push_back(baseIndex + idx);

  baseIndex = static_cast<uint32_t>(vertices.size());
  vertices.insert(vertices.end(), uiVertices.begin(), uiVertices.end());
  for (uint16_t idx : uiIndices)
    indices.push_back(baseIndex + idx);

  renderer.updateVertexBuffer(vertices);
  renderer.updateIndexBuffer(indices);
  renderer.updateUniformBuffer(0); // Update UBO
}
