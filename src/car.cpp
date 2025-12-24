#include "car.h"
#include <algorithm>

Car::Car(float x, float y, int r, int g, int b)
    : x(x), y(y)
    , velocityX(0), velocityY(0)
    , angle(0)
    , angularVelocity(0)
    , colorR(r), colorG(g), colorB(b)
{
}

void Car::update(float deltaTime, const Input& input) {
    // Get input
    float acceleration = 0;
    float turnAmount = 0;
    
    if (input.isForward()) {
        acceleration = ACCELERATION;
    }
    if (input.isBackward()) {
        acceleration = -BRAKE_FORCE;
    }
    if (input.isLeft()) {
        turnAmount = -TURN_SPEED;
    }
    if (input.isRight()) {
        turnAmount = TURN_SPEED;
    }
    
    // Calculate current speed
    float speed = std::sqrt(velocityX * velocityX + velocityY * velocityY);
    
    // Apply turning (only when moving)
    if (speed > 10.0f) {
        angle += turnAmount * deltaTime * (speed / MAX_SPEED);
    }
    
    // Apply acceleration in the direction the car is facing
    if (acceleration != 0) {
        velocityX += std::cos(angle) * acceleration * deltaTime;
        velocityY += std::sin(angle) * acceleration * deltaTime;
    }
    
    // Apply friction
    velocityX *= FRICTION;
    velocityY *= FRICTION;
    
    // Limit max speed
    speed = std::sqrt(velocityX * velocityX + velocityY * velocityY);
    if (speed > MAX_SPEED) {
        velocityX = (velocityX / speed) * MAX_SPEED;
        velocityY = (velocityY / speed) * MAX_SPEED;
    }
    
    // Update position
    x += velocityX * deltaTime;
    y += velocityY * deltaTime;
}

void Car::render(Renderer& renderer, const Camera& camera) {
    // Convert world coordinates to screen coordinates
    float screenX = x - camera.getX();
    float screenY = y - camera.getY();
    
    // Draw car as a rotated rectangle
    float carLength = 24.0f;
    float carWidth = 14.0f;
    
    // Calculate corners of the car
    float cosA = std::cos(angle);
    float sinA = std::sin(angle);
    
    SDL_FPoint points[5];
    
    // Front
    points[0].x = screenX + cosA * carLength / 2;
    points[0].y = screenY + sinA * carLength / 2;
    
    // Front-right
    float perpX = -sinA * carWidth / 2;
    float perpY = cosA * carWidth / 2;
    
    points[1].x = points[0].x + perpX;
    points[1].y = points[0].y + perpY;
    
    // Back-right
    points[2].x = screenX - cosA * carLength / 2 + perpX;
    points[2].y = screenY - sinA * carLength / 2 + perpY;
    
    // Back-left
    points[3].x = screenX - cosA * carLength / 2 - perpX;
    points[3].y = screenY - sinA * carLength / 2 - perpY;
    
    // Front-left
    points[4].x = points[0].x - perpX;
    points[4].y = points[0].y - perpY;
    
    renderer.drawPolygon(points, 5, colorR, colorG, colorB);
    
    // Draw direction indicator (front of car)
    renderer.drawLine(screenX, screenY, points[0].x, points[0].y, 255, 255, 255);
}

void Car::setPosition(float newX, float newY) {
    x = newX;
    y = newY;
}

void Car::setVelocity(float vx, float vy) {
    velocityX = vx;
    velocityY = vy;
}

void Car::applyImpulse(float fx, float fy) {
    velocityX += fx;
    velocityY += fy;
}
