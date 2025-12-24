#ifndef TRACK_H
#define TRACK_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "renderer.h"
#include "camera.h"
#include "car.h"

using json = nlohmann::json;

enum class TileType {
    GRASS,
    TRACK,
    START_FINISH,
    CHECKPOINT,
    JUMP,
    WALL
};

struct Tile {
    TileType type;
    float x, y;
    float width, height;
    float angle;
};

struct Point2D {
    float x, y;
};

class Track {
public:
    Track();
    
    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename);
    
    void render(Renderer& renderer, const Camera& camera);
    void checkCollisions(Car& car);
    
    Point2D getStartPosition(int index) const;
    
    void addTile(TileType type, float x, float y, float width, float height, float angle = 0);
    void clear();
    
    const std::vector<Tile>& getTiles() const { return tiles; }
    
private:
    std::vector<Tile> tiles;
    std::vector<Point2D> startPositions;
    
    void renderTile(const Tile& tile, Renderer& renderer, const Camera& camera);
    bool isOnTrack(float x, float y) const;
};

#endif // TRACK_H
