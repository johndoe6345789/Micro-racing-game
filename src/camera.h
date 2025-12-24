#ifndef CAMERA_H
#define CAMERA_H

#include <SDL2/SDL.h>

class Camera {
public:
    Camera(int screenWidth, int screenHeight);
    
    void setPosition(float x, float y);
    void followTarget(float targetX, float targetY);
    void update(float deltaTime);
    
    void apply(SDL_Renderer* renderer);
    void reset(SDL_Renderer* renderer);
    
    float getX() const { return x; }
    float getY() const { return y; }
    float getZoom() const { return zoom; }
    
private:
    float x, y;
    float targetX, targetY;
    float zoom;
    
    int screenWidth, screenHeight;
    
    static constexpr float LERP_SPEED = 5.0f;
};

#endif // CAMERA_H
