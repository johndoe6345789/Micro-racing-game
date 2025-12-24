#ifndef AI_BOT_H
#define AI_BOT_H

#include "car.h"
#include "track.h"
#include <memory>
#include <vector>

class AIBot {
public:
    AIBot(float x, float y, int difficulty);
    
    void update(float deltaTime, const Track& track);
    void render(Renderer& renderer, const Camera& camera);
    
    Car& getCar() { return *car; }
    const Car& getCar() const { return *car; }
    
private:
    std::unique_ptr<Car> car;
    int difficulty;
    
    // AI state
    float targetX, targetY;
    int waypointIndex;
    std::vector<Point2D> waypoints;
    
    void updateWaypoint(const Track& track);
    void calculateInput(float& forward, float& turn);
};

#endif // AI_BOT_H
