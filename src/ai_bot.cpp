#include "ai_bot.h"
#include <cmath>
#include <algorithm>

// Simple AI Input implementation
class AIInput : public Input {
public:
    AIInput() : Input() {
        forwardPressed = false;
        backwardPressed = false;
        leftPressed = false;
        rightPressed = false;
    }
    
    void setForward(bool val) { forwardPressed = val; }
    void setBackward(bool val) { backwardPressed = val; }
    void setLeft(bool val) { leftPressed = val; }
    void setRight(bool val) { rightPressed = val; }
    
    bool isForward() const override { return forwardPressed; }
    bool isBackward() const override { return backwardPressed; }
    bool isLeft() const override { return leftPressed; }
    bool isRight() const override { return rightPressed; }
    
private:
    bool forwardPressed;
    bool backwardPressed;
    bool leftPressed;
    bool rightPressed;
};

AIBot::AIBot(float x, float y, int difficulty)
    : difficulty(difficulty)
    , targetX(x), targetY(y)
    , waypointIndex(0)
{
    // Create AI car with red color
    car = std::make_unique<Car>(x, y, 255, 50, 50);
    
    // Generate simple circular waypoints for now
    float centerX = 640;
    float centerY = 360;
    float radius = 300;
    
    for (int i = 0; i < 8; ++i) {
        float angle = (i / 8.0f) * 2.0f * M_PI;
        waypoints.push_back({
            centerX + std::cos(angle) * radius,
            centerY + std::sin(angle) * radius
        });
    }
}

void AIBot::update(float deltaTime, const Track& track) {
    updateWaypoint(track);
    
    // Calculate direction to target
    float dx = targetX - car->getX();
    float dy = targetY - car->getY();
    float targetAngle = std::atan2(dy, dx);
    
    // Calculate angle difference
    float angleDiff = targetAngle - car->getAngle();
    
    // Normalize angle to [-PI, PI]
    while (angleDiff > M_PI) angleDiff -= 2 * M_PI;
    while (angleDiff < -M_PI) angleDiff += 2 * M_PI;
    
    // Create AI input
    AIInput input;
    input.setForward(true); // Always accelerate
    
    // Steering based on difficulty
    float steerThreshold = 0.1f / difficulty; // Higher difficulty = more precise
    
    if (angleDiff > steerThreshold) {
        input.setRight(true);
    } else if (angleDiff < -steerThreshold) {
        input.setLeft(true);
    }
    
    // Slow down on sharp turns
    if (std::abs(angleDiff) > M_PI / 3) {
        input.setForward(false);
        input.setBackward(true);
    }
    
    car->update(deltaTime, input);
}

void AIBot::render(Renderer& renderer, const Camera& camera) {
    car->render(renderer, camera);
    
    // Debug: Draw target waypoint
    float screenX = targetX - camera.getX();
    float screenY = targetY - camera.getY();
    renderer.drawCircle(screenX, screenY, 5, 255, 255, 0);
}

void AIBot::updateWaypoint(const Track& track) {
    // Check if reached current waypoint
    float dx = targetX - car->getX();
    float dy = targetY - car->getY();
    float distance = std::sqrt(dx * dx + dy * dy);
    
    if (distance < 50.0f) {
        // Move to next waypoint
        waypointIndex = (waypointIndex + 1) % waypoints.size();
        if (waypointIndex < waypoints.size()) {
            targetX = waypoints[waypointIndex].x;
            targetY = waypoints[waypointIndex].y;
        }
    }
}

void AIBot::calculateInput(float& forward, float& turn) {
    forward = 1.0f;
    
    float dx = targetX - car->getX();
    float dy = targetY - car->getY();
    float targetAngle = std::atan2(dy, dx);
    float angleDiff = targetAngle - car->getAngle();
    
    while (angleDiff > M_PI) angleDiff -= 2 * M_PI;
    while (angleDiff < -M_PI) angleDiff += 2 * M_PI;
    
    turn = std::clamp(angleDiff * 2.0f, -1.0f, 1.0f);
}
