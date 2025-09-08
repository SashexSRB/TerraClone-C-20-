#include "World.h"
#include <iostream>
#include <stdexcept>
#include <unordered_map>

std::unordered_map<uint16_t, TileProperties> TileRegistry::tileTypes;
std::unordered_map<uint16_t, TileProperties> TileRegistry::wallTypes;

void TileRegistry::initialize() {
  const float tileSize = 8.0f / 256.0f;

  static const std::vector<TileDefinition> tileDefs = {
      {0, "Air", 0, 0, false, 0.0f},
      {1, "Dirt", 1, 0, true, 0.5f},
      {2, "Stone", 2, 0, true, 0.5f},
      {3, "Grass", 3, 0, true, 0.5f},
  };

  static const std::vector<TileDefinition> wallDefs = {
      {0, "None", 0, 1, false, 1.0f},
      {1, "StoneWall", 1, 1, false, 1.0f},
  };

  for (const auto &def : tileDefs) {
    tileTypes[def.id] = {def.name,
                         {def.texX * tileSize, def.texY * tileSize},
                         def.isSolid,
                         def.zValue};
  }

  for (const auto &def : wallDefs) {
    wallTypes[def.id] = {
        def.name,
        {def.texX * tileSize, def.texY * tileSize},
        def.isSolid,
        def.zValue,
    };
  }
}

World::World(int w, int h) : width(w), height(h) {
  tiles.resize(w, std::vector<Tile>(h));
}

Tile &World::getTile(int x, int y) { return tiles[x][y]; }

void World::generate() {
  for (int x = 0; x < width; ++x) {
    for (int y = 0; y < height; ++y) {
      Tile &tile = tiles[x][y];
      if (y < height / 2) {
        tile.tileId = 0;
        tile.isActive = false;
      } else if (y == height / 2) {
        tile.tileId = 3;
        tile.isActive = true;
      } else if (y < height / 2 + 5) {
        tile.tileId = 1;
        tile.isActive = true;
      } else {
        tile.tileId = 2;
        tile.isActive = true;
      }
      tile.wallId = (y > height / 2) ? 1 : 0;
    }
  }
}

void World::generateVertices(std::vector<Vertex> &vertices,
                             std::vector<uint32_t> &indices) {
  vertices.clear();
  indices.clear();
  const float tileSize = 16.0f;            // Pixels per tile in world
  const float texTileSize = 8.0f / 256.0f; // 8x8 tiles in atlas
  const size_t maxVertices = 1000000;      // uint16_t limit
  for (int x = 0; x < width; ++x) {
    for (int y = 0; y < height; y++) {
      const Tile &tile = tiles[x][y];
      if (!tile.isActive && tile.wallId == 0)
        continue;
      if (vertices.size() >= maxVertices - 4)
        throw std::runtime_error("Too many vertices for uint16_t indices");
      if (tile.wallId != 0) {
        const TileProperties &props = TileRegistry::wallTypes[tile.wallId];
        uint32_t baseIndex = static_cast<uint32_t>(vertices.size());
        vertices.push_back({{x * tileSize, y * tileSize},
                            props.zValue,
                            {1.0f, 1.0f, 1.0f},
                            props.texCoord + glm::vec2(0.0f, texTileSize)});
        vertices.push_back(
            {{(x + 1) * tileSize, y * tileSize},
             props.zValue,
             {1.0f, 1.0f, 1.0f},
             props.texCoord + glm::vec2(texTileSize, texTileSize)});
        vertices.push_back({{(x + 1) * tileSize, (y + 1) * tileSize},
                            props.zValue,
                            {1.0f, 1.0f, 1.0f},
                            props.texCoord + glm::vec2(texTileSize, 0.0f)});
        vertices.push_back({{x * tileSize, (y + 1) * tileSize},
                            props.zValue,
                            {1.0f, 1.0f, 1.0f},
                            props.texCoord});
        indices.insert(indices.end(),
                       {baseIndex, baseIndex + 1, baseIndex + 2, baseIndex + 2,
                        baseIndex + 3, baseIndex});
      }

      if (tile.isActive) {
        const TileProperties &props = TileRegistry::tileTypes[tile.tileId];
        uint32_t baseIndex = static_cast<uint32_t>(vertices.size());
        vertices.push_back({{x * tileSize, y * tileSize},
                            props.zValue,
                            {1.0f, 1.0f, 1.0f},
                            props.texCoord + glm::vec2(0.0f, texTileSize)});
        vertices.push_back(
            {{(x + 1) * tileSize, y * tileSize},
             props.zValue,
             {1.0f, 1.0f, 1.0f},
             props.texCoord + glm::vec2(texTileSize, texTileSize)});
        vertices.push_back({{(x + 1) * tileSize, (y + 1) * tileSize},
                            props.zValue,
                            {1.0f, 1.0f, 1.0f},
                            props.texCoord + glm::vec2(texTileSize, 0.0f)});
        vertices.push_back({{x * tileSize, (y + 1) * tileSize},
                            props.zValue,
                            {1.0f, 1.0f, 1.0f},
                            props.texCoord});
        indices.insert(indices.end(),
                       {baseIndex, baseIndex + 1, baseIndex + 2, baseIndex + 2,
                        baseIndex + 3, baseIndex});
      }
    }
  }
  std::cout << "World generated: " << vertices.size() << " vertices, "
            << indices.size() << " indics.\n";
}
