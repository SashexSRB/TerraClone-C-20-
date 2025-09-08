#pragma once

#include "../Engine/VlkRenderer.h"
#include "Player.h"
#include "World.h"
#include <GLFW/glfw3.h>

class Game {
public:
  Game(GLFWwindow *window, VlkRenderer &renderer);
  Game(const Game &) = delete;
  Game &operator=(const Game &) = delete;
  void run();
  void notifyWorldChanged();

private:
  VlkRenderer &renderer;
  GLFWwindow *window;
  World world;
  Player player;
  std::vector<Vertex> worldVertices;
  std::vector<uint32_t> worldIndices;
  bool worldChanged = true;
  void update(float deltaTime);
  void handleInput();
  void updateBuffers();
};
