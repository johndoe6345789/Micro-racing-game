#include "renderer.h"
#include <cmath>

Renderer::Renderer(SDL_Renderer* sdlRenderer)
    : renderer(sdlRenderer)
{
}

Renderer::~Renderer() {
}

void Renderer::drawLine(float x1, float y1, float x2, float y2, int r, int g, int b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderDrawLine(renderer, (int)x1, (int)y1, (int)x2, (int)y2);
}

void Renderer::drawRect(float x, float y, float w, float h, int r, int g, int b, bool filled) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_Rect rect = { (int)x, (int)y, (int)w, (int)h };
    
    if (filled) {
        SDL_RenderFillRect(renderer, &rect);
    } else {
        SDL_RenderDrawRect(renderer, &rect);
    }
}

void Renderer::drawCircle(float cx, float cy, float radius, int r, int g, int b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    
    int x = radius;
    int y = 0;
    int err = 0;
    
    while (x >= y) {
        drawCirclePoints(cx, cy, x, y);
        
        if (err <= 0) {
            y += 1;
            err += 2 * y + 1;
        }
        
        if (err > 0) {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}

void Renderer::drawCirclePoints(int cx, int cy, int x, int y) {
    SDL_RenderDrawPoint(renderer, cx + x, cy + y);
    SDL_RenderDrawPoint(renderer, cx - x, cy + y);
    SDL_RenderDrawPoint(renderer, cx + x, cy - y);
    SDL_RenderDrawPoint(renderer, cx - x, cy - y);
    SDL_RenderDrawPoint(renderer, cx + y, cy + x);
    SDL_RenderDrawPoint(renderer, cx - y, cy + x);
    SDL_RenderDrawPoint(renderer, cx + y, cy - x);
    SDL_RenderDrawPoint(renderer, cx - y, cy - x);
}

void Renderer::drawPolygon(SDL_FPoint* points, int count, int r, int g, int b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    
    // Draw filled polygon using scanline algorithm (simplified)
    // For now, just draw the outline
    for (int i = 0; i < count - 1; ++i) {
        SDL_RenderDrawLine(renderer, (int)points[i].x, (int)points[i].y, 
                      (int)points[i + 1].x, (int)points[i + 1].y);
    }
    SDL_RenderDrawLine(renderer, (int)points[count - 1].x, (int)points[count - 1].y,
                  (int)points[0].x, (int)points[0].y);
}

void Renderer::renderText(const std::string& text, int x, int y, int r, int g, int b, float scale) {
    // Simple bitmap-style text rendering
    // For a real game, use SDL_ttf
    // For now, just draw a placeholder rectangle
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    
    int charWidth = 8 * scale;
    int charHeight = 12 * scale;
    
    for (size_t i = 0; i < text.length(); ++i) {
        SDL_Rect rect = { 
            x + (int)(i * charWidth), 
            y, 
            (int)(charWidth - 2), 
            (int)charHeight 
        };
        SDL_RenderDrawRect(renderer, &rect);
    }
}
