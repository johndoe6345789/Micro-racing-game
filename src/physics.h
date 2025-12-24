#ifndef PHYSICS_H
#define PHYSICS_H

#include <cmath>

class Physics {
public:
    static bool circleCollision(float x1, float y1, float r1, 
                               float x2, float y2, float r2);
    
    static bool rectCollision(float x1, float y1, float w1, float h1,
                             float x2, float y2, float w2, float h2);
    
    static float distance(float x1, float y1, float x2, float y2);
    
    static void normalize(float& x, float& y);
};

#endif // PHYSICS_H
