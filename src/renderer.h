#ifndef RENDERER_H
#define RENDERER_H

#include <SDL3/SDL.h>
#include <string>

class Renderer {
public:
    Renderer(SDL_Renderer* sdlRenderer);
    ~Renderer();
    
    void drawLine(float x1, float y1, float x2, float y2, int r, int g, int b);
    void drawRect(float x, float y, float w, float h, int r, int g, int b, bool filled = true);
    void drawCircle(float x, float y, float radius, int r, int g, int b);
    void drawPolygon(SDL_FPoint* points, int count, int r, int g, int b);
    
    void renderText(const std::string& text, int x, int y, int r, int g, int b, float scale = 1.0f);
    
private:
    SDL_Renderer* renderer;
    
    void drawCirclePoints(int cx, int cy, int x, int y);
};

#endif // RENDERER_H
