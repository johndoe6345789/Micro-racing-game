#ifndef CAR_H
#define CAR_H

#include "renderer.h"
#include "camera.h"
#include "input.h"
#include <cmath>

class Car {
public:
    Car(float x, float y, int r, int g, int b);
    
    void update(float deltaTime, const Input& input);
    void render(Renderer& renderer, const Camera& camera);
    
    float getX() const { return x; }
    float getY() const { return y; }
    float getAngle() const { return angle; }
    float getVelocityX() const { return velocityX; }
    float getVelocityY() const { return velocityY; }
    
    void setPosition(float newX, float newY);
    void setVelocity(float vx, float vy);
    void applyImpulse(float fx, float fy);
    
    // Collision
    float getRadius() const { return 12.0f; }
    
private:
    float x, y;
    float velocityX, velocityY;
    float angle;
    float angularVelocity;
    
    // Visual
    int colorR, colorG, colorB;
    
    // Physics constants
    static constexpr float ACCELERATION = 500.0f;
    static constexpr float BRAKE_FORCE = 800.0f;
    static constexpr float TURN_SPEED = 3.5f;
    static constexpr float MAX_SPEED = 400.0f;
    static constexpr float FRICTION = 0.98f;
};

#endif // CAR_H
