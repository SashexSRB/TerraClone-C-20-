#include "Player.h"

void generatePlayerVertices(const Player &player, std::vector<Vertex> &vertices,
                            std::vector<uint32_t> &indices) {
  vertices.clear();
  indices.clear();
  const float size = 32.0f; // Player sprite size
  float z = 0.2f;           // Player in front of tiles
  const float texTileSize = 8.0f / 256.0f;
  vertices.push_back({{player.position.x, player.position.y},
                      z,
                      {1.0f, 1.0f, 1.0f},
                      {0.0f, texTileSize}});
  vertices.push_back({{player.position.x + size, player.position.y},
                      z,
                      {1.0f, 1.0f, 1.0f},
                      {texTileSize, texTileSize}});
  vertices.push_back({{player.position.x + size, player.position.y + size},
                      z,
                      {1.0f, 1.0f, 1.0f},
                      {texTileSize, 0.0f}});
  vertices.push_back({{player.position.x, player.position.y + size},
                      z,
                      {1.0f, 1.0f, 1.0f},
                      {0.0f, 0.0f}});
  indices = {0, 1, 2, 2, 3, 0};
}

void generateInventoryVertices(const Inventory &inventory, float screenWidth,
                               float screenHeight,
                               std::vector<Vertex> &vertices,
                               std::vector<uint32_t> &indices) {
  vertices.clear();
  indices.clear();
  const float slotSize = 40.0f; // UI slot size
  const float texTileSize = 8.0f / 256.0f;
  float y = screenHeight - slotSize; // Bottom of screen
  for (size_t i = 0; i < inventory.items.size() && i < 10; ++i) {
    float x = i * slotSize;
    uint32_t tileId = inventory.items[i].first;
    const TileProperties &props = TileRegistry::tileTypes[tileId];
    uint32_t baseIndex = static_cast<uint32_t>(vertices.size());
    vertices.push_back({{x, y},
                        0.0f,
                        {1.0f, 1.0f, 1.0f},
                        props.texCoord + glm::vec2(0.0f, texTileSize)});
    vertices.push_back({{x + slotSize, y},
                        0.0f,
                        {1.0f, 1.0f, 1.0f},
                        props.texCoord + glm::vec2(texTileSize, texTileSize)});
    vertices.push_back({{x + slotSize, y + slotSize},
                        0.0f,
                        {1.0f, 1.0f, 1.0f},
                        props.texCoord + glm::vec2(texTileSize, 0.0f)});
    vertices.push_back(
        {{x, y + slotSize}, 0.0f, {1.0f, 1.0f, 1.0f}, props.texCoord});
    indices.insert(indices.end(), {baseIndex, baseIndex + 1, baseIndex + 2,
                                   baseIndex + 2, baseIndex + 3, baseIndex});
  }
}
