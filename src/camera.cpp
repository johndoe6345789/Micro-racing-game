#include "camera.h"
#include <algorithm>

Camera::Camera(int screenWidth, int screenHeight)
    : x(0), y(0)
    , targetX(0), targetY(0)
    , zoom(1.0f)
    , screenWidth(screenWidth)
    , screenHeight(screenHeight)
{
}

void Camera::setPosition(float newX, float newY) {
    x = newX - screenWidth / 2;
    y = newY - screenHeight / 2;
    targetX = x;
    targetY = y;
}

void Camera::followTarget(float targetPosX, float targetPosY) {
    targetX = targetPosX - screenWidth / 2;
    targetY = targetPosY - screenHeight / 2;
}

void Camera::update(float deltaTime) {
    // Smooth camera movement using lerp
    float lerpFactor = std::min(1.0f, LERP_SPEED * deltaTime);
    
    x += (targetX - x) * lerpFactor;
    y += (targetY - y) * lerpFactor;
}

void Camera::apply(SDL_Renderer* renderer) {
    // For SDL2, camera offset is handled in render code
    // by subtracting camera position from world coordinates
}

void Camera::reset(SDL_Renderer* renderer) {
    // Reset viewport if needed
}
