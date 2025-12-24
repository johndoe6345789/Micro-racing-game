#include "physics.h"

bool Physics::circleCollision(float x1, float y1, float r1, 
                             float x2, float y2, float r2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float distSq = dx * dx + dy * dy;
    float radiusSum = r1 + r2;
    return distSq < radiusSum * radiusSum;
}

bool Physics::rectCollision(float x1, float y1, float w1, float h1,
                           float x2, float y2, float w2, float h2) {
    return x1 < x2 + w2 &&
           x1 + w1 > x2 &&
           y1 < y2 + h2 &&
           y1 + h1 > y2;
}

float Physics::distance(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return std::sqrt(dx * dx + dy * dy);
}

void Physics::normalize(float& x, float& y) {
    float length = std::sqrt(x * x + y * y);
    if (length > 0) {
        x /= length;
        y /= length;
    }
}
