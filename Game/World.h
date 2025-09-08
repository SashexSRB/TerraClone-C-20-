#pragma once

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <vector>

struct Vertex {
  glm::vec2 pos;
  float z;
  glm::vec3 color;
  glm::vec2 texCoord;
};

struct Tile {
  uint16_t tileId = 0;
  uint16_t wallId = 0;
  bool isActive = false;
};

struct TileProperties {
  std::string name;
  glm::vec2 texCoord;
  bool isSolid = true;
  float zValue = 0.5f;
};

struct TileDefinition {
  uint16_t id;
  std::string name;
  int texX, texY;
  bool isSolid;
  float zValue;
};

class TileRegistry {
public:
  static std::unordered_map<uint16_t, TileProperties> tileTypes;
  static std::unordered_map<uint16_t, TileProperties> wallTypes;
  static void initialize();
};

class World {
private:
  std::vector<std::vector<Tile>> tiles;
  int width, height;

public:
  World(int w, int h);
  Tile &getTile(int x, int y);
  int getWidth() const { return width; }
  int getHeight() const { return height; }
  void generate();
  void generateVertices(std::vector<Vertex> &vertices,
                        std::vector<uint16_t> &indices);
};
