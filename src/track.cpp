#include "track.h"
#include <fstream>
#include <cmath>
#include <iostream>

Track::Track() {
}

bool Track::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open track file: " << filename << std::endl;
        return false;
    }
    
    try {
        json trackData;
        file >> trackData;
        
        // Load tiles
        tiles.clear();
        if (trackData.contains("tiles")) {
            for (const auto& tileData : trackData["tiles"]) {
                Tile tile;
                tile.type = static_cast<TileType>(tileData["type"].get<int>());
                tile.x = tileData["x"].get<float>();
                tile.y = tileData["y"].get<float>();
                tile.width = tileData["width"].get<float>();
                tile.height = tileData["height"].get<float>();
                tile.angle = tileData.value("angle", 0.0f);
                tiles.push_back(tile);
            }
        }
        
        // Load start positions
        startPositions.clear();
        if (trackData.contains("startPositions")) {
            for (const auto& pos : trackData["startPositions"]) {
                startPositions.push_back({
                    pos["x"].get<float>(),
                    pos["y"].get<float>()
                });
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing track file: " << e.what() << std::endl;
        return false;
    }
}

bool Track::saveToFile(const std::string& filename) {
    json trackData;
    
    // Save tiles
    json tilesArray = json::array();
    for (const auto& tile : tiles) {
        json tileData;
        tileData["type"] = static_cast<int>(tile.type);
        tileData["x"] = tile.x;
        tileData["y"] = tile.y;
        tileData["width"] = tile.width;
        tileData["height"] = tile.height;
        tileData["angle"] = tile.angle;
        tilesArray.push_back(tileData);
    }
    trackData["tiles"] = tilesArray;
    
    // Save start positions
    json startArray = json::array();
    for (const auto& pos : startPositions) {
        json posData;
        posData["x"] = pos.x;
        posData["y"] = pos.y;
        startArray.push_back(posData);
    }
    trackData["startPositions"] = startArray;
    
    // Write to file
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to save track file: " << filename << std::endl;
        return false;
    }
    
    file << trackData.dump(2);
    return true;
}

void Track::render(Renderer& renderer, const Camera& camera) {
    for (const auto& tile : tiles) {
        renderTile(tile, renderer, camera);
    }
}

void Track::renderTile(const Tile& tile, Renderer& renderer, const Camera& camera) {
    float screenX = tile.x - camera.getX();
    float screenY = tile.y - camera.getY();
    
    int r, g, b;
    switch (tile.type) {
        case TileType::GRASS:
            r = 50; g = 150; b = 50;
            break;
        case TileType::TRACK:
            r = 80; g = 80; b = 80;
            break;
        case TileType::START_FINISH:
            r = 255; g = 255; b = 255;
            break;
        case TileType::CHECKPOINT:
            r = 255; g = 200; b = 0;
            break;
        case TileType::JUMP:
            r = 150; g = 75; b = 0;
            break;
        case TileType::WALL:
            r = 100; g = 50; b = 50;
            break;
        default:
            r = 128; g = 128; b = 128;
    }
    
    renderer.drawRect(screenX, screenY, tile.width, tile.height, r, g, b);
    
    // Draw border for track tiles
    if (tile.type == TileType::TRACK || tile.type == TileType::START_FINISH) {
        renderer.drawRect(screenX, screenY, tile.width, tile.height, 
                         r - 30, g - 30, b - 30, false);
    }
}

void Track::checkCollisions(Car& car) {
    bool onTrack = isOnTrack(car.getX(), car.getY());
    
    if (!onTrack) {
        // Apply friction when off track
        car.setVelocity(car.getVelocityX() * 0.9f, car.getVelocityY() * 0.9f);
    }
    
    // Check for wall collisions
    for (const auto& tile : tiles) {
        if (tile.type == TileType::WALL) {
            float dx = car.getX() - (tile.x + tile.width / 2);
            float dy = car.getY() - (tile.y + tile.height / 2);
            float distance = std::sqrt(dx * dx + dy * dy);
            
            float minDist = car.getRadius() + std::min(tile.width, tile.height) / 2;
            
            if (distance < minDist && 
                car.getX() > tile.x && car.getX() < tile.x + tile.width &&
                car.getY() > tile.y && car.getY() < tile.y + tile.height) {
                // Push car out of wall
                float angle = std::atan2(dy, dx);
                car.setPosition(
                    tile.x + tile.width / 2 + std::cos(angle) * minDist,
                    tile.y + tile.height / 2 + std::sin(angle) * minDist
                );
                // Bounce back
                car.setVelocity(-car.getVelocityX() * 0.5f, -car.getVelocityY() * 0.5f);
            }
        }
    }
}

bool Track::isOnTrack(float x, float y) const {
    for (const auto& tile : tiles) {
        if ((tile.type == TileType::TRACK || 
             tile.type == TileType::START_FINISH ||
             tile.type == TileType::CHECKPOINT ||
             tile.type == TileType::JUMP) &&
            x > tile.x && x < tile.x + tile.width &&
            y > tile.y && y < tile.y + tile.height) {
            return true;
        }
    }
    return false;
}

Point2D Track::getStartPosition(int index) const {
    if (index >= 0 && index < startPositions.size()) {
        return startPositions[index];
    }
    return {400, 300}; // Default position
}

void Track::addTile(TileType type, float x, float y, float width, float height, float angle) {
    tiles.push_back({type, x, y, width, height, angle});
}

void Track::clear() {
    tiles.clear();
    startPositions.clear();
}
