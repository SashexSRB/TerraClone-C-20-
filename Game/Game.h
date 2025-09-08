#pragma once

#include "../Engine/VlkRenderer.h"
#include "Player.h"
#include "World.h"
#include <GLFW/glfw3.h>

class Game {
public:
  Game(VlkRenderer &renderer, GLFWwindow *window);
  void run();

private:
  VlkRenderer &renderer;
  GLFWwindow *window;
  World world;
  Player player;
  void update(float deltaTime);
  void handleInput();
  void updateBuffers();
};
